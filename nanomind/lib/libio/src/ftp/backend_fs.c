#include <inttypes.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include <malloc.h>

#include <freertos/FreeRTOS.h>

#include <util/crc32.h>
#include <uffs/uffs_fd.h>
#include <util/log.h>

#include <ftp/ftp_server.h>

/* Chunk status markers */
static const char const *packet_missing = "-";
static const char const *packet_ok = "+";

struct ftp_uffs_state {
	/* Current bitmap and file */
	FILE *fd_file;
	FILE *fd_map;
	char *mem_map;

	/* Current file info */
	char file_name[FTP_PATH_LENGTH];
	uint32_t file_size;
	uint32_t file_chunk_size;
	uint32_t file_chunks;

	/* Directory pointer used in listing */
	DIR *dirp;
	char dirpath[FTP_PATH_LENGTH];
};

ftp_return_t ftp_uffs_abort(void *state);
ftp_return_t ftp_uffs_timeout(void *state);

/* Calculate CRC of current file */
int file_crc(void *state, uint32_t *crc_arg) {

	struct ftp_uffs_state *uffs_state = (struct ftp_uffs_state *)state;

	if (!uffs_state)
		return FTP_RET_INVAL;

	/* Flush file before calculating CRC */
	if (fflush(uffs_state->fd_file) != 0)
		printf("Flush error\r\n");
	if (fsync(fileno(uffs_state->fd_file)) != 0)
		printf("Fsync error\r\n");
	if (fseek(uffs_state->fd_file, 0, SEEK_SET) != 0)
		printf("Fseek error\r\n");


	//printf("File size to check %"PRIu32", state %p, fd %p\r\n", uffs_state->file_size, uffs_state, uffs_state->fd_file);
	uint8_t byte;
	uint32_t crc = 0xFFFFFFFF;
	while(fread(&byte, 1, 1, uffs_state->fd_file) == 1)
		crc = chksum_crc32_step(crc, byte);
	crc = (crc ^ 0xFFFFFFFF);

	*crc_arg = crc;

	//printf("Backend CRC 0x%"PRIx32" (%p), 0x%"PRIx32"\r\n", *crc_arg, crc_arg, crc);

	return 0;

}

ftp_return_t ftp_uffs_init(void **state) {

	if (state) {
		struct ftp_uffs_state *uffs_state = pvPortMalloc(sizeof(struct ftp_uffs_state));
		if (uffs_state) {
			uffs_state->fd_file = NULL;
			uffs_state->fd_map = NULL;
			uffs_state->mem_map = NULL;
			uffs_state->dirp = NULL;
			*state = uffs_state;
			return FTP_RET_OK;
		}
	}

	return FTP_RET_NOMEM;

}

ftp_return_t ftp_uffs_release(void *state) {

	if (state)
		vPortFree(state);

	return FTP_RET_OK;
}

ftp_return_t ftp_uffs_upload(void *state, const char const *path, uint32_t memaddr, uint32_t size, uint32_t chunk_size) {

	/* Map file name */
	char map[100];

	struct ftp_uffs_state *uffs_state = (struct ftp_uffs_state*)state;

	if (!uffs_state)
		return FTP_RET_INVAL;

	uffs_state->file_size = size;
	uffs_state->file_chunk_size = chunk_size;
	uffs_state->file_chunks = (size + chunk_size - 1) / chunk_size;
	strncpy(uffs_state->file_name, path, FTP_PATH_LENGTH);
	uffs_state->file_name[FTP_PATH_LENGTH - 1] = '\0';

	/* Abort if a previous transfer was incomplete */
	if (uffs_state->fd_file || uffs_state->fd_map || uffs_state->mem_map) {
		printf("Aborting previous transfer %p %p %p\r\n", uffs_state->fd_file, uffs_state->fd_map, uffs_state->mem_map);
		ftp_uffs_timeout(state);
		printf("ftp_uffs_abort complete\r\n");
	}

	/* Try to open file */
	uffs_state->fd_file = fopen((const char *) path, "r+");
	if (uffs_state->fd_file == NULL) {
		/* Create new file */
		uffs_state->fd_file = fopen((const char *) path, "w+");
		if (uffs_state->fd_file == NULL) {
			printf("Server: Failed to create data file\r\n");
			return FTP_RET_IO;
		}
	}

	/* Allocate memory map */
	if (uffs_state->mem_map == NULL) {
		uffs_state->mem_map = malloc(uffs_state->file_chunks);
		if (uffs_state->mem_map == NULL) {
			fclose(uffs_state->fd_file);
			uffs_state->fd_file = NULL;
			return FTP_RET_NOSPC;
		}
	}

	/* Try to create a new map file */
	strncpy(map, path, FTP_PATH_LENGTH);
	strcpy((char *) map + strlen(path) - 4, ".MAP");

	/* Check if file already exists */
	uffs_state->fd_map = fopen(map, "r+");
	if (uffs_state->fd_map == NULL || fread(uffs_state->mem_map, 1, uffs_state->file_chunks, uffs_state->fd_map) != uffs_state->file_chunks) {
		unsigned int i;

		/* Close file if already open but data could not be read */
		if (uffs_state->fd_map) {
			fclose(uffs_state->fd_map);
			uffs_state->fd_map = NULL;
		}

		/* Create new file */
		uffs_state->fd_map = fopen(map, "w+");
		if (uffs_state->fd_map == NULL) {
			printf("Failed to create bitmap %s\r\n", map);
			fclose(uffs_state->fd_file);
			uffs_state->fd_file = NULL;
			free(uffs_state->mem_map);
			uffs_state->mem_map = NULL;
			return FTP_RET_IO;
		}

		/* Clear contents */
		for (i = 0; i < uffs_state->file_chunks; i++) {
			if (fwrite(packet_missing, 1, 1, uffs_state->fd_map) < 1) {
				printf("Failed to clear bitmap\r\n");
				fclose(uffs_state->fd_map);
				uffs_state->fd_map = NULL;
				fclose(uffs_state->fd_file);
				uffs_state->fd_file = NULL;
				free(uffs_state->mem_map);
				uffs_state->mem_map = NULL;
				return FTP_RET_IO;
			}
			uffs_state->mem_map[i] = *packet_missing;
		}

		fflush(uffs_state->fd_map);
		fsync(fileno(uffs_state->fd_map));
	}

	return FTP_RET_OK;

}

ftp_return_t ftp_uffs_download(void *state, const char const *path, uint32_t memaddr, uint32_t memsize, uint32_t chunk_size, uint32_t *size, uint32_t *crc) {

	struct ftp_uffs_state *uffs_state = (struct ftp_uffs_state *)state;

	if (!uffs_state)
		return FTP_RET_INVAL;

	/* Try to open file */
	uffs_state->fd_file = fopen((const char *) path, "r+");
	if (uffs_state->fd_file == NULL)
		return FTP_RET_NOENT;

	/* Read size */
	struct stat statbuf;
	if (stat(path, &statbuf) != 0)
		return FTP_RET_IO;

	*size = statbuf.st_size;
	uffs_state->file_size = *size;
	uffs_state->file_chunk_size = chunk_size;
	uffs_state->file_chunks = (uffs_state->file_size + chunk_size - 1) / chunk_size;
	strncpy(uffs_state->file_name, path, FTP_PATH_LENGTH);
	uffs_state->file_name[FTP_PATH_LENGTH - 1] = '\0';

	/* Calculate CRC */
	if (file_crc(state, crc) != 0)
		return FTP_RET_IO;
	else
		return FTP_RET_OK;

}

ftp_return_t ftp_uffs_write_chunk(void *state, uint32_t chunk, uint8_t *data, uint32_t size) {

	struct ftp_uffs_state *uffs_state = (struct ftp_uffs_state *)state;

	if (!uffs_state)
		return FTP_RET_INVAL;

	/* Write to file */
	if (fseek(uffs_state->fd_file, chunk * uffs_state->file_chunk_size, SEEK_SET) != 0)
		goto chunk_write_error;
	if (fwrite(data, 1, size, uffs_state->fd_file) != size)
		goto chunk_write_error;

	return FTP_RET_OK;

chunk_write_error:
	printf("Filesystem write error\r\n");
	fclose(uffs_state->fd_file);
	uffs_state->fd_file = NULL;
	return FTP_RET_IO;

}

ftp_return_t ftp_uffs_read_chunk(void *state, uint32_t chunk, uint8_t *data, uint32_t size) {

	struct ftp_uffs_state *uffs_state = (struct ftp_uffs_state *)state;

	if (!uffs_state)
		return FTP_RET_INVAL;

	/* Read from file */
	if (fseek(uffs_state->fd_file, chunk * uffs_state->file_chunk_size, SEEK_SET) != 0)
		goto chunk_read_error;
	if (fread(data, 1, size, uffs_state->fd_file) != size)
		goto chunk_read_error;

	return FTP_RET_OK;

chunk_read_error:
	printf("Filesystem read error (chunk %"PRIu32", size %"PRIu32", file chunk size %"PRIu32")\r\n",
			chunk, size, uffs_state->file_chunk_size);
	fclose(uffs_state->fd_file);
	uffs_state->fd_file = NULL;
	return FTP_RET_IO;

}

ftp_return_t ftp_uffs_get_status(void *state, uint32_t chunk, int *status) {

	struct ftp_uffs_state *uffs_state = (struct ftp_uffs_state *)state;

	if (!uffs_state || !status)
		return FTP_RET_INVAL;

	*status = (uffs_state->mem_map[chunk] == *packet_ok);
	return FTP_RET_OK;

}

ftp_return_t ftp_uffs_set_status(void *state, uint32_t chunk) {

	struct ftp_uffs_state *uffs_state = (struct ftp_uffs_state *)state;

	if (!uffs_state)
		return FTP_RET_INVAL;

	/* Write to map file */
	if (fseek(uffs_state->fd_map, chunk, SEEK_SET) != 0)
		goto status_write_error;
	if (fwrite(packet_ok, 1, 1, uffs_state->fd_map) != 1)
		goto status_write_error;

	fflush(uffs_state->fd_map);
	fsync(fileno(uffs_state->fd_map));

	/* Mark in RAM map */
	uffs_state->mem_map[chunk] = *packet_ok;

	return FTP_RET_OK;

status_write_error:
	printf("Failed to write status for chunk %"PRIu32" %p\r\n", chunk, uffs_state->fd_map);
	fclose(uffs_state->fd_map);
	uffs_state->fd_map = NULL;
	return FTP_RET_IO;

}

ftp_return_t ftp_uffs_get_crc(void *state, uint32_t *crc) {

	return (crc != NULL && file_crc(state, crc) == 0) ? FTP_RET_OK : FTP_RET_INVAL;

}

ftp_return_t ftp_uffs_done(void *state) {

	struct ftp_uffs_state *uffs_state = (struct ftp_uffs_state *)state;

	if (!uffs_state)
		return FTP_RET_INVAL;

	/* Close files */
	if (uffs_state->fd_file) {
		fclose(uffs_state->fd_file);
		uffs_state->fd_file = NULL;
	}
	if (uffs_state->fd_map) {
		fclose(uffs_state->fd_map);
		uffs_state->fd_map = NULL;
	}
	if (uffs_state->mem_map) {
		free(uffs_state->mem_map);
		uffs_state->mem_map = NULL;
	}
	if (uffs_state->dirp) {
		closedir(uffs_state->dirp);
		uffs_state->dirp = NULL;
	}

	/* Generate map path */
	char map[100];
	strncpy(map, uffs_state->file_name, FTP_PATH_LENGTH);
	strcpy((char *) map + strlen(uffs_state->file_name) - 4, ".MAP");

	/* Remove map */
	remove(map);

	return FTP_RET_OK;

}

ftp_return_t ftp_uffs_abort(void *state) {

	struct ftp_uffs_state *uffs_state = (struct ftp_uffs_state *)state;

	if (!uffs_state)
		return FTP_RET_INVAL;

	/* Close files */
	if (uffs_state->fd_file) {
		fclose(uffs_state->fd_file);
		uffs_state->fd_file = NULL;
	}
	if (uffs_state->fd_map) {
		fclose(uffs_state->fd_map);
		uffs_state->fd_map = NULL;
	}
	if (uffs_state->mem_map) {
		free(uffs_state->mem_map);
		uffs_state->mem_map = NULL;
	}
	if (uffs_state->dirp) {
		closedir(uffs_state->dirp);
		uffs_state->dirp = NULL;
	}

	/* Generate map path */
	char map[100];
	uffs_state->file_name[FTP_PATH_LENGTH-1] = '\0';
	snprintf(map, FTP_PATH_LENGTH, "%sm", uffs_state->file_name);

	/* Remove map */
	if (remove(map) != 0) {
		printf("Failed to remove %s\r\n", map);
		return FTP_RET_IO;
	}

	/* Remove file */
	if (remove(uffs_state->file_name) != 0) {
		printf("Failed to remove %s\r\n", uffs_state->file_name);
		return FTP_RET_IO;
	}

	return FTP_RET_OK;

}

ftp_return_t ftp_uffs_timeout(void *state) {

	struct ftp_uffs_state *uffs_state = (struct ftp_uffs_state *)state;

	if (!uffs_state)
		return FTP_RET_INVAL;

	/* Close files */
	if (uffs_state->fd_file) {
		fclose(uffs_state->fd_file);
		uffs_state->fd_file = NULL;
	}
	if (uffs_state->fd_map) {
		fclose(uffs_state->fd_map);
		uffs_state->fd_map = NULL;
	}
	if (uffs_state->dirp) {
		closedir(uffs_state->dirp);
		uffs_state->dirp = NULL;
	}
	if (uffs_state->mem_map) {
		free(uffs_state->mem_map);
		uffs_state->mem_map = NULL;
	}

	return FTP_RET_OK;

}

ftp_return_t ftp_uffs_remove(void *state, char *path) {

	if (remove(path) != 0) {
		/* We assume that file does not exist */
		printf("path: %s\r\n", path);
		return FTP_RET_NOENT;
	} else {
		return FTP_RET_OK;
	}

}

ftp_return_t ftp_uffs_move(void *state, char *from, char *to) {

	if (rename(from, to) != 0) {
		/* We assume that the file does not exist */
		printf("from: %s to: %s\r\n", from, to);
		return FTP_RET_NOENT;
	} else {
		return FTP_RET_OK;
	}

}

ftp_return_t ftp_uffs_list(void *state, char *path, uint16_t *entries) {

	struct ftp_uffs_state *uffs_state = (struct ftp_uffs_state *)state;

	if (!uffs_state || !path || !entries)
		return FTP_RET_INVAL;

	/* Copy entries */
	struct dirent *ent;
	*entries = 0;

	uffs_state->dirp = opendir(path);
	if (uffs_state->dirp == NULL)
		return FTP_RET_NOENT;

	strncpy(uffs_state->dirpath, path, FTP_PATH_LENGTH);

	/* Count entries */
	while ((ent = readdir(uffs_state->dirp)))
		(*entries)++;

	/* Rewind for entry function */
	rewinddir(uffs_state->dirp);

	return FTP_RET_OK;

}

ftp_return_t ftp_uffs_entry(void *state, ftp_list_entry_t *ent) {

	struct dirent *dent;
	struct stat stat_buf;
	char buf[FTP_PATH_LENGTH];
	char *sub;

	struct ftp_uffs_state *uffs_state = (struct ftp_uffs_state *)state;

	if (!uffs_state)
		return FTP_RET_INVAL;

	/* ent is allowed to be NULL, if an entry should be skipped */
	if (uffs_state->dirp == NULL)
		return FTP_RET_IO;

	/* Loop through directories */
	if ((dent = readdir(uffs_state->dirp))) {
		if (ent == NULL)
			return FTP_RET_INVAL;

		/* Name */
		strncpy(ent->path, dent->d_name, FTP_PATH_LENGTH);
		ent->path[FTP_PATH_LENGTH-1] = '\0';

		/* Type */
		ent->type = (dent->d_type & DT_DIR) ? FTP_LIST_DIR : FTP_LIST_FILE;

		/* Size */
		strcpy(buf, uffs_state->dirpath);
		sub = buf;
		if (uffs_state->dirpath[strlen(uffs_state->dirpath)-1] != '/')
			sub = strcat(buf, "/");
		sub = strcat(sub, dent->d_name);
		if (!(dent->d_type & FILE_ATTR_DIR))
			stat(sub, &stat_buf);
		ent->size = (dent->d_type & DT_DIR ? 0 : (uint32_t)stat_buf.st_size);
	} else {
		closedir(uffs_state->dirp);
		uffs_state->dirp = NULL;
		return FTP_RET_NOENT;
	}

	return FTP_RET_OK;

}

ftp_backend_t backend_uffs = {
	.init			= ftp_uffs_init,
	.release		= ftp_uffs_release,
	.upload 		= ftp_uffs_upload,
	.download 		= ftp_uffs_download,
	.chunk_write	= ftp_uffs_write_chunk,
	.chunk_read 	= ftp_uffs_read_chunk,
	.status_get 	= ftp_uffs_get_status,
	.status_set 	= ftp_uffs_set_status,
	.list 			= ftp_uffs_list,
	.entry 			= ftp_uffs_entry,
	.remove			= ftp_uffs_remove,
	.move			= ftp_uffs_move,
	.crc 			= ftp_uffs_get_crc,
	.abort 			= ftp_uffs_abort,
	.done 			= ftp_uffs_done,
	.timeout		= ftp_uffs_timeout,
};
