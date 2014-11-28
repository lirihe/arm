/**
 * @file gatoss.h
 * GATOSS Microcontroller
 *
 * @author Johan De Claville Christiansen
 * Copyright 2011 GomSpace ApS. All rights reserved.
 */

#ifndef GATOSS_H_
#define GATOSS_H_

#include <stdint.h>
#include <stdbool.h>

#define GATOSS_NODE							4

/** task_server.c port numbers */
#define GATOSS_PORT_FTP						7
#define GATOSS_PORT_POWER					8
#define GATOSS_PORT_HK						9
#define GATOSS_PORT_LOAD_FPGA				10
#define GATOSS_PORT_LOAD_UC					11
#define GATOSS_PORT_THRESHOLD				12
#define GATOSS_PORT_SEL_ADC					13
#define GATOSS_PORT_RSH						14
#define GATOSS_PORT_RECOVER_FS				15
#define GATOSS_PORT_CONF					16
#define GATOSS_PORT_RESTORE					17

/** task fp_server port number */
#define GATOSS_PORT_FP						18

/** task_data.c port numbers */
#define GATOSS_PORT_LIST_SHORT				20
#define GATOSS_PORT_LIST_FULL				21
#define GATOSS_PORT_PLANE_FULL				22
#define GATOSS_PORT_PLANE_POSITIONS			23
#define GATOSS_PORT_PLANE_CONTACTS			24
#define GATOSS_PORT_FILE_ALL_FRAMES			25
#define GATOSS_PORT_FILE_ALL_POSITIONS		26
#define GATOSS_PORT_FILE_PLANE				27
#define GATOSS_PORT_FILE_RAW_SAMPLES		28
#define GATOSS_PORT_BLOB_LS					29
#define GATOSS_PORT_DB_STORE				30

/** Ground station addr and port numbers for passive mode */
#define GATOSS_PASSIVE_GND_ADDR				11
#define GATOSS_PASSIVE_PORT_LIST_SHORT		11
#define GATOSS_PASSIVE_PORT_LIST_FULL		12
#define GATOSS_PASSIVE_PORT_PLANE			13
#define GATOSS_PASSIVE_PORT_LIVE_POSITION	14

/** System configuration struct (network type) */
typedef struct __attribute__((__packed__)) {
	uint8_t sd_enable;									//! Enable SD card 1 = ON, 0 = OFF
	uint8_t fpga_image;									//! Fallback to embedded image 0 = COR, 1 = CRC
	uint8_t fpga_wdt_enable;							//! Disable this to override CRC check on FPGA (0 = off)
	int32_t fpga_threshold_a;							//! Default value for fpga threshold A
	int32_t fpga_threshold_b;							//! Default value for fpga threshold B
	int32_t fpga_threshold_count;						//! Threshold count for starting auto search. 0 = off.
	uint8_t fpga_adc;									//! 0 for ADC A and 1 for ADC B
	uint16_t fpga_adc_switch_interval;					//! Interval between the ADC in use is switched. Set to zero for deactivating this feature
	uint16_t passive_inhibit;							//! Inhibit the passive task for X seconds on data-activity
	uint16_t passive_interval_list_short;				//! Interval in seconds
	uint16_t passive_interval_list_full;				//! Interval in seconds
	uint16_t passive_interval_plane;					//! Interval in seconds
	uint16_t passive_live_min_tx_interval;				//! Minimum interval for live positions [s]
	uint16_t passive_live_interval;						//! Call live_transmit at these intervals [s]
	uint16_t passive_filter_list_short;					//! Filter in passive mode (on/off)
	uint16_t passive_filter_list_full;					//! Filter in passive mode (on/off)
	uint16_t garbage_collector_interval;				//! Garbage collector run interval in sec. <= 0 -> off.
	uint16_t garbage_plane_inactive_interval;			//!	Time in seconds a plane can be inactive before it is removed
	uint16_t garbage_plane_no_lock_inactive_interval;	//! Time in seconds a place without lock can be inactive before it is removed
	uint16_t garbage_plane_frame_count;					//! Plane is removed is it has less than this number of frames and inactive for frame_interval
	uint16_t garbage_plane_frame_interval;				//! Time in seconds plane with less than frame_count can be inactive before it is removed
	uint16_t garbage_pos_lifetime;						//! Plane position lifetime in minutes
	uint16_t garbage_frame_lifetime;					//! Plane frame lifetime in minutes
	uint8_t plane_enable_frame_log;						//! Plane database will log frames if > 0
	uint8_t plane_position_log_interval;				//! Only log positions that are spaced more than this interval apart [s]
} gatoss_conf_t;

/** General shared datatype */
typedef struct __attribute__((__packed__)) {
	float lat;											//! Decoded lattitude
	float lon;											//! Decoded longtiude
	uint32_t altitude;									//! Decoded altitude
	uint32_t timestamp;									//! Time that the position was decoded
} gatoss_pos_t;

/** Compressed version of position */
typedef struct __attribute__((__packed__)) {
	uint8_t compos[7];									//! compos[7] { lon : 23, lat : 22, alt : 11 }
	uint32_t timestamp;									//! Non compressed 32-bit unix timestamp
} gatoss_compos_t;

/** Set node */
void gatoss_set_node(uint8_t node);

/** Datatypes used on: GATOSS_PORT_POWER */
#define GATOSS_POWER_CHANNEL_FPGA_1V2 			(1 << 0)
#define GATOSS_POWER_CHANNEL_FPGA_2V5 			(1 << 1)
#define GATOSS_POWER_CHANNEL_MAIN_3V3 			(1 << 2)
#define GATOSS_POWER_CHANNEL_MAIN_RF 			(1 << 3)
#define GATOSS_POWER_CHANNEL_RX0	 			(1 << 4)
#define GATOSS_POWER_CHANNEL_RX1 				(1 << 5)
#define GATOSS_POWER_CHANNEL_ADC0 				(1 << 6)
#define GATOSS_POWER_CHANNEL_ADC1 				(1 << 7)
#define GATOSS_POWER_CHANNEL_SD 				(1 << 8)
#define GATOSS_POWER_CHANNEL_ALL 				(0x1FF)

struct __attribute__((__packed__)) gatoss_power_req {
	uint16_t channels;
	uint8_t state;
};

struct __attribute__((__packed__)) gatoss_power_state {
	uint16_t channels_state;
};

struct __attribute__((__packed__)) gatoss_hk_req {
	uint8_t	save;
};


/** Datatypes used on: GATOSS_PORT_HK */
typedef struct __attribute__((__packed__)) gatoss_hk {
	uint16_t current_1v2;							//! This is sampled by HK task
	uint16_t current_2v5;							//! This is sampled by HK task
	uint16_t current_3v3_fpga;						//! This is sampled by HK task
	uint16_t current_3v3_adc;						//! This is sampled by HK task
	uint16_t current_5v0_board;						//! This is sampled by HK task
	uint16_t current_3v3_board;						//! This is sampled by HK task
	uint16_t average_fps_10sec;						//! This is sampled by HK task
	uint16_t average_fps_1min;						//! This is sampled by HK task
	uint16_t average_fps_5min;						//! This is sampled by HK task
	uint16_t plane_count;							//! This is updated by parser
	uint32_t frame_count;							//! This is updated by parser
	uint32_t crc_corrected;							//! This is updated by parser
	uint32_t last_icao24;
	gatoss_pos_t last_pos;
	uint32_t bootcount;								//! This is updated by userpage
	uint16_t bootcause;								//! This is updated by userpage
	uint32_t current_time;							//! This is set by the server
	uint32_t tot_frames;
	uint32_t tot_planes;
	uint32_t tot_crc_corrected;
	uint32_t fpga_crc_cnt;
	uint32_t flash_last_write;
} gatoss_hk_t;

struct __attribute__((__packed__)) gatoss_hk_log {
	unsigned int frames_sec;						//! This is sampled by HK task
};

/** Datatypes used on: GATOSS_PORT_<LOAD|SWLOAD> */
#define GATOSS_LOAD_FILE_LEN_MAX			20
struct __attribute__((__packed__)) gatoss_load_req {
	uint8_t load_now;								//! Setting to 1 loads image now
	uint8_t store_as_dfl;							//! Setting to 1 stores filename as default
	uint8_t path[GATOSS_LOAD_FILE_LEN_MAX];			//! Filename (without partition)
};

/** Datatype used in: LIST_<SHORT|FULL> */
struct __attribute__((__packed__)) gatoss_list_req {
	uint32_t filtering;								//! 1 for filtering
};

/** Datatype used in: LIST_SHORT */
struct  __attribute__((__packed__)) gatoss_list_short {
	uint8_t  count;									//! Number of planes in packet
	uint8_t  end_flag;								//! This is the last packet;
	struct  __attribute__((__packed__)) {
		uint8_t icao24[3];							//! The ICAO24 address
		gatoss_compos_t compos;						//! Last known position in compressed format
	} planes[15];
};

/** Datatype used in: LIST_FULL */
struct __attribute__((__packed__)) gatoss_list_full {
	uint8_t  count;									//! Number of planes in packet
	uint8_t  end_flag;								//! This is the last packet;
	struct __attribute__((__packed__)) {
		uint32_t icao24;							//! The ICAO24 address
		uint8_t  id[9];								//! The aircraft ID (8 bytes + null)
		uint16_t frame_counter;						//! The total number of frames received (CRC OK)
		uint16_t position_counter;					//! The total number of positions decoded
		uint32_t first_contact;						//! The time of first contact
		uint32_t last_contact;						//! The time of last received frame
		gatoss_compos_t compos;						//! The Last known position in compressed format
		uint16_t heading;							//! The heading in degr * 150
		uint16_t speed;								//! The speed in kts
	} planes[5];
};

/** Datatype used in: PLANE_FULL */
struct __attribute__((__packed__)) gatoss_plane_full_req {
	uint32_t icao24;								//! The ICAO24 address
};

struct __attribute__((__packed__)) gatoss_plane_full {
	uint32_t icao24;								//! The ICAO24 address
	uint8_t  id[9];									//! The aircraft ID (8 bytes + null)
	uint16_t frame_counter;							//! The total number of frames received (CRC OK)
	uint16_t position_counter;						//! The total number of positions decoded
	uint32_t first_contact;							//! The time of first contact
	uint32_t last_contact;							//! The time of last received frame
	gatoss_compos_t compos;							//! The Last known position in compressed format
	uint16_t heading;								//! The heading in degr * 150
	uint16_t speed;									//! The speed in km/t
	gatoss_compos_t positions[15];					//! An array of positions
};

/** Datatype used in: PLANE_POSITIONS */
struct __attribute__((__packed__)) gatoss_plane_positions_req {
	uint32_t icao24;								//! The ICAO24 address
	uint32_t min_distance;							//! Position decimation step in km
};

struct __attribute__((__packed__)) gatoss_plane_positions {
	uint32_t icao24;								//! The ICAO24 address
	uint8_t  count;									//! Number of positions in packet
	uint8_t  end_flag;								//! This is the last packet;
	gatoss_compos_t positions[18];					//! An array of positions
};

/** Datatype used in: PLANE_CONTACTS */
struct __attribute__((__packed__)) gatoss_plane_contacts_req {
	uint32_t icao24;								//! The ICAO24 address
};

struct __attribute__((__packed__)) gatoss_plane_contacts {
	uint32_t icao24;								//! The ICAO24 address
	uint8_t  count;									//! Number of positions in packet
	uint8_t  end_flag;								//! This is the last packet;
	uint32_t abs_time;								//! Absolute time to which the follwing deltas refer
	uint16_t delta_time[95];						//! Time delta of next elements
};

/** Datatype used in: FILE_PLANE_<POSITIONS|FRAMES> */
struct __attribute__((__packed__)) gatoss_file_plane_req {
	uint32_t icao24;								//! The ICAO24 address
	uint8_t format;									//! The storage format [0 = positions, 1 = frames]
	uint32_t begin_time;							//! Filter elements by timestamp start
	uint32_t end_time;								//! Filter elements by timestamp end
	uint32_t max_size;								//! Max file size in bytes
	char path[50];									//! Filename to store to
};

struct __attribute__((__packed__)) gatoss_file_plane_reply {
	uint32_t mem_addr;								//! Address of stored blob
	uint32_t mem_size;								//! Size of stored blob
};

struct __attribute__((__packed__)) gatoss_file_plane_position {
	gatoss_compos_t compos;							//! Position in compressed format
};

struct __attribute__((__packed__)) gatoss_file_plane_frame {
	uint32_t timestamp;								//! Time of reception
	uint8_t data[7];								//! Raw ME data field
};

/** Datatype used in: FILE_ALL_FRAMES */
struct __attribute__((__packed__)) gatoss_file_all_frames_req {
	uint32_t start_time;							//! Unix timestamp
	uint32_t run_duration;							//! Run time in seconds
	uint32_t max_size;								//! Max file size in bytes
	char path[50];									//! Filename to store to
};

struct __attribute__((__packed__)) gatoss_file_all_frame {
	uint32_t timestamp;								//! Time of reception
	char data[14];									//! Decoded bytes from the FPGA including icao24 and crc
};

/** Datatype used in: FILE_ALL_POSITIONS */
struct __attribute__((__packed__)) gatoss_file_all_positions_req {
	uint32_t start_time;							//! Unix timestamp
	uint32_t run_duration;							//! Run time in seconds
	uint32_t max_size;								//! Max file size in bytes
	char path[50];									//! Filename to store to
};

struct __attribute__((__packed__)) gatoss_file_all_position {
	uint32_t icao24;								//! The ICAO24 address
	gatoss_compos_t position;						//! A single position in compressed format
};

/** Datatype used in: FILE_RAW_SAMPLES */
struct __attribute__((__packed__)) gatoss_file_raw_samples_req {
	uint8_t noise_sample;							//! Start with one noise sample, 0 = no, 1 = yes
	uint32_t timeout;								//! Stop sampling after this timeout (ms)
	uint32_t max_count;								//! Stop sampling after x frames
	uint32_t max_size;								//! Max file size in bytes
	int32_t threshold;								//! Trigger threshold
	char path[50];									//! Filename to store to
};

struct __attribute__((__packed__)) gatoss_file_raw_samples {
	uint32_t timestamp;								//! Time of reception
	uint8_t compressed_samples[5075];				//! 10-bit samples packed next to neck (4 samples in 5 bytes)
};

/** Datatype used in: LIVE_POSITIONS */
struct __attribute__((__packed__)) gatoss_live_position {
	uint32_t icao24;								//! The ICAO24 address
	gatoss_compos_t position;						//! A single position in compressed format
};

/** Dataype used on: GATOSS_PORT_BLOB_LS */
#define GATOSS_BLOB_FLAG_MAGIC_PLANE_POS			1
#define GATOSS_BLOB_FLAG_MAGIC_PLANE_FRM			2
#define GATOSS_BLOB_FLAG_MAGIC_ALL_FRM				3
#define GATOSS_BLOB_FLAG_MAGIC_ALL_POS				4
#define GATOSS_BLOB_FLAG_MAGIC_RAW_SAMPLE			5
#define GATOSS_BLOB_FLAG_MAGIC_MASK					0xF
#define GATOSS_BLOB_FLAG_COMPRESSED_MASK			(1 << 4)

struct __attribute__((__packed__)) gatoss_blob_list {
	uint8_t  count;									//! Number of planes in packet
	uint8_t  end_flag;								//! This is the last packet;
	struct __attribute__((__packed__)) {
		uint16_t id;								//! The idendtifier is the number in the index
		uint32_t mem_addr;							//! Memory address of data
		uint32_t mem_used;							//! Actually used size in blob
		uint32_t mem_size;							//! The size allocated in mem
		uint32_t creation_time;						//! Time of creation
		uint16_t user_flags;						//! Arbitrary user flags
		uint8_t name[9];							//! "shortname" to help remember
	} blobs[7];
};

/** Datatype used on: GATOSS_PORT_DB_STORE */
struct __attribute__((__packed__)) gatoss_db_store_req {
	uint8_t type;									//! 0 = load, 1 = store
	char path[50];								//! Filename
};

/**
 * Send a command to power on a channel
 * @param channels byte representing 1 or more channels, use OR to combine multiple.
 * @return standard transaction error: 0 = success
 */
int gatoss_power_on(uint16_t channels);

/**
 * Send a command to power off a channel
 * @param channels byte representing 1 or more channels, use OR to combine multiple.
 * @return standard transaction error: 0 = success
 */
int gatoss_power_off(uint16_t channels);

/**
 * Send a command to turn power either on or off.
 * @param channels byte representing 1 or more channels, use OR to combine multiple.
 * @param state 0 = off, 1 = on
 * @return standard transaction error: 0 = success
 */
int gatoss_power(uint16_t channels, int state, uint16_t * channels_state);

/**
 * Send a housekeeping request
 * @param hk pointer to hk struct
 * @return standard transaction error: 0 = success
 */
int gatoss_hk(struct gatoss_hk * hk, uint8_t save);

/**
 * Load new firmware into the FPGA
 * @param path filename to load (must be present on SD card)
 * @param load_now load into fpga when request is received
 * @param store_as_dfl store as the default boot image
 * @return 0 = ok
 */
int gatoss_load_fpga(char * path, uint8_t load_now, uint8_t store_as_dfl);

/**
 * Load new firmware into the Microprocessor
 * @param path filename to load (must be present on SD card)
 * @param load_now load into fpga when request is received
 * @param store_as_dfl store as the default boot image
 * @return 0 = ok
 */
int gatoss_load_uc(char * path, uint8_t load_now, uint8_t store_as_dfl);

/**
 * Change the synchronization threshold on the FPGA by amount
 * @param amount amount the change the threshold with
 * @return -1 if err, -2 if network error, 0 if ok
 */
int gatoss_threshold(int32_t amount);

/**
 * Select which adc to use
 * @param adc 0 for ADC A and 1 for ADC B
 * @return -1 if err, -2 if network error, 0 if ok
 */
int gatoss_sel_adc(uint8_t adc);

/**
 * Send a command to format the filesytem
 * @return -2 if network err, result otherwise
 */
int gatoss_format_fs(void);

/**
 * Request a list of planes in short format
 * @return -1 if err
 */
int gatoss_list_short(uint32_t filtering);

/**
 * Request a list of planes in full format
 * @return -1 if err
 */
int gatoss_list_full(uint32_t filtering);

/**
 * Request plane information for specific ICAO24 address, or buffer index
 * @param plane pointer to output plane buffer
 * @param icao24 address if > 1000, buffer index if <= 1000;
 * @return > 0 = ok
 */
int gatoss_plane_full(struct gatoss_plane_full * plane, uint32_t icao24);

/**
 * Request a list of planes positions
 * @param icao24 address if > 1000, buffer index if <= 1000;
 * @param decimation step size in kilometers
 * @return -1 if err
 */
int gatoss_plane_positions(uint32_t icao24, uint32_t decimation_step);

/**
 * Request a list of plane contact timestamps
 * @param icao24 address if > 1000, buffer index if <= 1000;
 * @return -1 if err
 */
int gatoss_plane_contacts(uint32_t icao24);

/**
 * Store a current plane from RAM to Filesystem
 * @param icao24 address of plane or buffer index
 * @param format 0 = positions, 1 = frames
 * @param begin_time filter by start time
 * @param end_time filter by end time
 * @param max_size max file size, 0 = inf
 * @param path max 50 bytes
 * @return -1 if invalid request, -2 if network error, 0 if ok
 */
int gatoss_file_plane(uint32_t icao24, uint8_t format, uint32_t begin_time, uint32_t end_time, uint32_t max_size, char * path, struct gatoss_file_plane_reply * reply);

/**
 * Start the frame logging task at specified time for duration or untill max filesize
 * @param start_time unix timestamp in seconds since epoch, 0 = start now
 * @param run_duration in seconds, 0 = forever or untill max_size
 * @param max_size in bytes, 0 = forever or untill run_duration
 * @param path max 50 bytes
 * @return -1 if already running or error, -2 if network error, 0 if ok
 */
int gatoss_file_all_frames(uint32_t start_time, uint32_t run_duration, uint32_t max_size, char * path);

/**
 * Start the positions logging task at specified time for duration or untill max filesize
 * @param start_time unix timestamp in seconds since epoch, 0 = start now
 * @param run_duration in seconds, 0 = forever or untill max_size
 * @param max_size in bytes, 0 = forever or untill run_duration
 * @param path max 50 bytes
 * @return -1 if already running or error, -2 if network error, 0 if ok
 */
int gatoss_file_all_positions(uint32_t start_time, uint32_t run_duration, uint32_t max_size, char * path);

/**
 * Start the rawmode task ensuring only one instance
 * @param noise_sample start sampling by triggering 1 noise sample
 * @param timeout stop sampling after this timeout
 * @param max_count max number of frames to sample
 * @param path filename to store data to (max 50 bytes)
 * @param threshold FPGA synchronization threshold
 * @return -1 if already running or error, -2 if network error, 0 if ok
 */
int gatoss_file_raw_samples(uint8_t noise_sample, uint32_t timeout, uint32_t max_count, int32_t threshold, char * path);

/**
 * Compress a pos_t to a compos_to
 * This compresses 12 bytes (lat, lon, alt) into 7 bytes
 * @param in pointer to pos_t
 * @param out pointer to compos_t
 */
void gatoss_position_compress(gatoss_pos_t * in, gatoss_compos_t * out);

/**
 * Decompress a compos_t to a pos_to
 * This decompresses 7 bytes (lat, lon, alt) into 12 bytes
 * @param in pointer to compos_t
 * @param out pointer to pos_t
 */
void gatoss_positions_decompress(gatoss_compos_t * in, gatoss_pos_t * out);


/**
 * Send a SET_CONF message
 * @param config pointer to config structure
 * @return result of csp_transaction
 */
int gatoss_conf_set(gatoss_conf_t * config);

/**
 * Send a GET_CONF message,
 * and wait for a response before returning.
 * The timeout is 5 seconds.
 * @param config pointer to where retrieved config will be stored
 * @return result of csp_transaction
 */
int gatoss_conf_get(gatoss_conf_t * config);

/**
 * Send a SET_CONF_RESTORE message
 * This will delete the current stored config
 * and return the device to factory settings. This is useful in the
 * event that an invalid configuration was sent to the node.
 * @return result of csp_transaction
 */
int gatoss_conf_restore(void);

/**
 * Do a pretty printout of a gatoss config
 * @param config pointer to config
 */
void gatoss_conf_print(gatoss_conf_t * config);

/**
 * Store entire database to file
 * @param path filename
 * @return 0 if ok, < 0 if not
 */
int gatoss_db_store(char * path);

/**
 * Load entire database from file
 * @param path filename
 * @return 0 if ok, < 0 if not
 */
int gatoss_db_load(char * path);

/**
 * List blobs
 *
 * @param blob_lst pointer to a blob_lst
 * @return result of csp_transaction
 */
int gatoss_blob_ls(void);

#endif /* GATOSS_H_ */
