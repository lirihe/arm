#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>

#include <dev/usart.h>
#include <dev/cpu.h>
#include <conf_gomspace.h>

/* libgomspace cannot depend on libstorage, so we have to declare these symbols weak */
__attribute__ ((weak)) int vfs_open(const char *path, int oflag, int mode);
__attribute__ ((weak)) int vfs_close(int fildes);
__attribute__ ((weak)) int vfs_read(int fildes, void *buf, int nbyte);
__attribute__ ((weak)) int vfs_write(int fildes, const void *buf, int nbyte);
__attribute__ ((weak)) off_t vfs_lseek(int fildes, off_t offset, int whence);
__attribute__ ((weak)) int vfs_fstat(int fildes, struct stat *buf);
__attribute__ ((weak)) int vfs_stat(const char *path, struct stat *buf);
__attribute__ ((weak)) int vfs_fsync(int fildes);
__attribute__ ((weak)) int vfs_unlink(const char *path);
__attribute__ ((weak)) int vfs_link(const char *oldpath, const char *newpath);
__attribute__ ((weak)) int vfs_rename(const char *oldpath, const char *newpath);

int _read(int file, void *ptr, size_t len) {

	if (file <= STDERR_FILENO) {
		*(char *) ptr = usart_getc(USART_CONSOLE);
		if ((*(char *) ptr != '\n') && (*(char *) ptr != '\r'))
			usart_putc(USART_CONSOLE, *(char *) ptr);
		return 1;
	} else {
		if (vfs_read)
			return vfs_read(file, ptr, len);
		return -1;
	}

}

int _write(int file, const void *ptr, size_t len) {

	size_t i;

	if (file <= STDERR_FILENO) {
		for(i = 0; i < len; i++) {
			if (((const char *) ptr)[i] == '\n')
				usart_putc(USART_CONSOLE, '\r');
			usart_putc(USART_CONSOLE, ((const char *) ptr)[i]);
		}
		return len;
	} else {
		if (vfs_write)
			return vfs_write(file, ptr, len);
		return -1;
	}

}

off_t _lseek(int file, off_t ptr, int dir) {
	if (file <= STDERR_FILENO) {
		return -1;
	} else {
		if (vfs_lseek)
			return vfs_lseek(file, ptr, dir);
		return -1;
	}
}

int _close(int file) {
	if (file <= STDERR_FILENO) {
		return -1;
	} else {
		if (vfs_close)
			return vfs_close(file);
		return -1;
	}
}

int _open(const char *name, int flags, ...) {
	if (vfs_open)
		return vfs_open(name, flags, 0);
	return -1;
}

int _link(const char *oldpath, const char *newpath) {
	if (vfs_link)
		return vfs_link(oldpath, newpath);
	return -1;
}

int _rename(const char *oldpath, const char *newpath) {
	if (vfs_rename)
		return vfs_rename(oldpath, newpath);
	return -1;
}

int _unlink(const char *pathname) {
	if (vfs_unlink)
		return vfs_unlink(pathname);
	return -1;
}

int fsync(int fd) {
	if (fd <= STDERR_FILENO) {
		return -1;
	} else {
		if (vfs_fsync)
			return vfs_fsync(fd);
		return -1;
	}
}

int _fstat(int fd, struct stat *st) {
	if (fd <= STDERR_FILENO) {
		return -1;
	} else {
		if (vfs_fstat)
			return vfs_fstat(fd, (struct stat *) st);
		return -1;
	}
}

int _stat(const char *path, struct stat *st) {
	if (vfs_stat)
		return vfs_stat(path, (struct stat *) st);
	return -1;
}

int _isatty(int fd) {
	if (fd <= STDERR_FILENO)
		return 1;
	else
		return 0;
}

void exit(int n) {
	printf("Exit %u\r\n", n);
	cpu_reset();
	while(1);
}

void _exit(int n) {
	printf("Exit %u\r\n", n);
	cpu_reset();
	while(1);
}

/* Register name faking - works in collusion with the linker.  */
register char * stack_ptr asm ("sp");

void *_sbrk(ptrdiff_t increment) {

	extern char __heap_start__, __heap_end__;
	static void *cur_heap_pos = 0;

	/* Initialize cur_heap_pos */
	if (cur_heap_pos == 0)
		cur_heap_pos = &__heap_start__;

	if ((cur_heap_pos + increment) > (void *) &__heap_end__) {
		printf("Warning: Heap is running full trying to allocate %i bytes!!!!\n", increment);
		printf("\tHeap start address\t= %p\n", &__heap_start__);
		printf("\tHeap end address\t= %p\n", &__heap_end__);
		printf("\tCurrent heap address\t= %p\n", cur_heap_pos);
		errno = ENOMEM;
		return (void *) -1;
	}

	void * old_heap_pos = cur_heap_pos;
	cur_heap_pos += increment;
	return old_heap_pos;

}

int _kill() {
	return -1;
}

int _getpid() {
	return 1;
}

int _gettimeofday() {
	return -1;
}
