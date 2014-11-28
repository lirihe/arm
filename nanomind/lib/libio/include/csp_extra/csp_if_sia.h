/*
 * csp_if_sia.h
 *
 *  Created on: Sep 16, 2010
 *      Author: q-web
 */

#ifndef CSP_IF_SIA_H_
#define CSP_IF_SIA_H_

#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include "csp/csp_platform.h"


#define SDCP_USE_ACK 1
#define CSP_SDCP_PACKETLOOP_TIMEOUT 2

/*
 * SIA interface
 */
/* Struct binding IP to node */
typedef struct csp_sia_route_s {
	char * ip;
} csp_sia_route_t;

/* Interface */
extern csp_iface_t csp_if_sia;

int csp_sia_init(uint16_t port, char * ip, char * privatekey);
int csp_sia_route_set(uint8_t node, char * IP);
int csp_sia_tx(csp_iface_t * interface, csp_packet_t * packet, uint32_t timeout);
void csp_sia_rx(csp_packet_t * packet);





/********************************************************************************
 * SDCP DRIVER																	*
 ********************************************************************************/



#define SDCP_KEEPALIVE_PERIODE 300 // Send each 5 min
#define SDCP_PUBLICKEY_NON 0

typedef enum csp_sdcp_conn_state_s {
	CSP_SDCP_STATE_OPEN		= 1,
	CSP_SDCP_STATE_CLOSING	= 0,
} csp_sdcp_conn_state_t;

/* Struct for linked list of active connections */
typedef struct csp_sdcp_conn_s {
	int sock;
	char ip[20];
	uint32_t sequence_tx;
	uint32_t sequence_rx;
	uint32_t publickey;
	sem_t lock;
	sem_t sendlock;
	pthread_mutex_t sig_mutex;
	pthread_cond_t signal;
	int packet_acked;
	csp_sdcp_conn_state_t state;
	uint32_t lastpacket;
	int lastpacket_time;
	struct csp_sdcp_conn_s *next;
} csp_sdcp_conn_t;

/* enum for types for SDCP packet */
typedef enum csp_sdcp_packet_type_s {
	CSP_SDCP_DATA 			= 0,
	CSP_SDCP_ACK 			= 1,
	CSP_SDCP_KEYEXCHANGE	= 2,
	CSP_SDCP_KEEPALIVE 		= 3,
	CSP_SDCP_CHECKERROR 	= 4,
	CSP_SDCP_SEQSYNC		= 5,
	CSP_SDCP_DISCONNECT 	= 6,
} csp_sdcp_packet_type;

/* Struct for SDCP packet */
typedef struct csp_sdcp_packet_s {
	struct __attribute__((__packed__)) {
		csp_sdcp_packet_type type 	: 3;
		uint8_t reserved			: 4;
		uint32_t length 			: 17;
	};
	uint32_t sequence;
	unsigned char checksum[20];
	uint8_t data[0];
} csp_sdcp_packet_t;

typedef void (*csp_sdcp_rx_func)(csp_packet_t * packet);

int csp_sdcp_init(uint16_t port, char * ip, csp_sdcp_rx_func rxRef, char* privatekey);
void csp_sdcp_setprivatekey(char * privatekey);
void* csp_sdcp_newconn_task(void * pvParameters);
void* csp_sdcp_rx_task(void * pvParameters);
void* csp_sdcp_keepaliveTask(void * pvParameters);

int csp_sdcp_tx(char * IPaddress, csp_packet_t * packet, unsigned int timeout);
csp_sdcp_conn_t * csp_sdcp_findconn(char * IPaddress);
csp_sdcp_conn_t* csp_sdcp_connect(char* IP);
void csp_sdcp_insert_checksum(csp_sdcp_packet_t * sdcp_packet, csp_sdcp_conn_t * connection);
int csp_sdcp_send_packet(csp_sdcp_conn_t * connection, csp_sdcp_packet_type type, void * data, uint32_t length);
void csp_sdcp_close(char * IP);

void csp_sdcp_conn_insert(csp_sdcp_conn_t * newNode);
void csp_sdcp_conn_remove(csp_sdcp_conn_t * conn);
csp_sdcp_conn_t * csp_sdcp_conn_find(char * IPaddress);



#endif /* CSP_IF_SIP_H_ */
