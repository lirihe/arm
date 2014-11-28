/*
Cubesat Space Protocol - A small network-layer protocol designed for Cubesats
Copyright (C) 2010 GomSpace ApS (gomspace.com)
Copyright (C) 2010 AAUSAT3 Project (aausat3.space.aau.dk)

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

Author: Martin Dam
*/


/* CSP includes */
#include <stdlib.h>
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <time.h>
#include <sys/time.h>
#include <dev/x86/ipsocket.h>

#include <csp/csp.h>
#include <csp/csp_platform.h>
#include <csp/csp_interface.h>
#include <csp_extra/csp_if_sia.h>

#include <util/log.h>

#include <csp/arch/csp_semaphore.h>
#include <csp/arch/csp_queue.h>

#include "../src/crypto/csp_sha1.h"
#include "../src/csp_route.h"

/** Interface definition */
csp_iface_t csp_if_sia = {
	.name = "SIA",
	.nexthop = csp_sia_tx,
	.split_horizon_off = 1,
};



/* Create routing table for static ip TCP/IP interface. Set to +3 so default route and DGS route can be set */
csp_sia_route_t sia_route[CSP_ID_HOST_MAX+3];
/* Semaphores for route list */
csp_bin_sem_handle_t sia_route_sem;

static uint8_t csp_sia_initated = 0;

static char csp_sia_myaddress[16];



/** Initiate static ip TCP/IP interface
 * Must be called BEFORE csp_sia_route_set()
 * @param uint16_t port (The port to start SDCP on)
 * @return int (-1 = error, 1 = OK)
 */
int csp_sia_init(uint16_t port, char * ip, char * privatekey) {

	csp_debug(CSP_INFO, "Starting SIA on address %s\r\n", ip);
	strcpy(csp_sia_myaddress, ip);

	/* Make it a "singleton" */
	if(csp_sia_initated == 1) return 1;
	csp_sia_initated = 1;

	/* Create semaphores */
	if(csp_bin_sem_create(&sia_route_sem) == CSP_SEMAPHORE_ERROR) {
		csp_debug(CSP_ERROR, "No more memory for SIP route semaphore\r\n");
		return -1;
	}

	/* Initiate the routing table by setting all to NULL */
	int i = 0;
	for(i = 0; i < CSP_ID_HOST_MAX+3; i++) {
		sia_route[i].ip = NULL;
	}
	// Initiate SDCP driver
	return csp_sdcp_init(port, ip, csp_sia_rx, privatekey);

}

/** Set a static route between a node and a IP
 * @param <uint8_t> node (allowed: 0-(CSP_ID_HOST_MAX+2))
 * @param <char *> IP (The IP of the node in quad dotted format)
 * @return int (0 = error, 1 = OK)
 */
int csp_sia_route_set(uint8_t node, char * IP) {
	if(csp_sia_initated == 0) return 0;

	if(node > CSP_DEFAULT_ROUTE+1) {
		csp_debug(CSP_WARN, "Node %i not within allowed parameters\r\n", node);
		return 0;
	}

	sem_wait(&sia_route_sem);
	if(IP == NULL) {
		if(sia_route[node].ip != NULL) {
			free(sia_route[node].ip);
		}
		sia_route[node].ip = NULL;
	} else {
		if(sia_route[node].ip != NULL) {
			free(sia_route[node].ip);
		}
		sia_route[node].ip = (char *)malloc(strlen(IP)+2);
		strcpy(sia_route[node].ip, IP);
	}
	sem_post(&sia_route_sem);



	csp_debug(CSP_INFO, "Created static IP route from node %i to IP %s\r\n", node, IP);

	return 1;
}

/**
 * SIA interface for TX
 * @param interface pointer to originating interface
 * @param csp_id_t idout (The header of the CSP packet)
 * @param csp_packet_t * packet (The packet content)
 * @param uint32_t timeout (The timeout in miliseconds)
 * @return int (0 = error, 1 = ok)
 */
int csp_sia_tx(csp_iface_t * interface, csp_packet_t * packet, uint32_t timeout) {

	if(csp_sia_initated == 0) return 0;

	char tmpIP[16];
	uint8_t nodeID;					/* Node ID for dest */
	int returnValue;
	int i;
	int nodeAvail = 0;

	if(packet->id.dst == CSP_BROADCAST_ADDR) {

		/* Save the outgoing id in the buffer */
		packet->id.ext = htonl(packet->id.ext);
		packet->length += 4;

		csp_debug(CSP_INFO, "Sending broadcast packet to all routes\r\n");

		/* Packet is a broadcast, run through the entire routing tabel and send to everyone including default route */
		// TODO: BUG: If there is set two routes to the same IP, there will be sent to packets to that destination.
		for(i = 0; i < CSP_ID_HOST_MAX+2; i++) {
			sem_wait(&sia_route_sem);
			if(sia_route[i].ip != NULL) {
				strcpy(tmpIP, sia_route[i].ip);
				nodeAvail = 1;
			} else {
				nodeAvail = 0;
			}
			sem_post(&sia_route_sem);
			if(nodeAvail == 1) {
				csp_sdcp_tx(tmpIP, packet, timeout);
			}
		}
		returnValue = 1; // Assume all is sent correct

	} else {
		/* Lookup the node in routing table */
		sem_wait(&sia_route_sem);
		if(sia_route[packet->id.dst].ip == NULL) {
			if(sia_route[CSP_DEFAULT_ROUTE].ip == NULL) {
				csp_debug(CSP_ERROR, "No route to %i nor default route\r\n", packet->id.dst);
				sem_post(&sia_route_sem);
				// Free the packet
				csp_buffer_free(packet);
				return 0;
			} else {
				nodeID = CSP_DEFAULT_ROUTE;
			}
		} else {
			nodeID = packet->id.dst;
		}
		strcpy(tmpIP, sia_route[nodeID].ip);
		sem_post(&sia_route_sem);


		/* Save the outgoing id in the buffer */
		packet->id.ext = htonl(packet->id.ext);
		packet->length += 4;

		returnValue = csp_sdcp_tx(tmpIP, packet, timeout);

	}
	// Free the packet
	csp_buffer_free(packet);

	if (returnValue == 1)
		return CSP_ERR_NONE;
	return CSP_ERR_TX;
}
/**
 * Rx function called by SDCP
 * @param csp_packet_t * packet (The received packet)
 */
void csp_sia_rx(csp_packet_t * packet) {


	/** SDCP packet checked. Unpack CSP packet and insert into router */
	/* Strip the CSP header off the length field before converting to CSP packet */
	packet->length -= 4;

	/* Convert the packet from network to host order */
	packet->id.ext = ntohl(packet->id.ext);

	/* Send packet into CSP,
	 * 	set second argument to NULL for allowing the router to send out on the same interface,
	 * third = NULL to indicate this is called from a task */
	csp_new_packet(packet, &csp_if_sia, NULL);
}

/*****************************************************************************************************************************
 *****************************************************************************************************************************
 ** 													START OF SDCP														**
 *****************************************************************************************************************************
 *****************************************************************************************************************************/

/* Create mounting point for linked list of open connections */
static csp_sdcp_conn_t * sdcp_conn = NULL;
/* Semaphores for connection list */
static csp_bin_sem_handle_t sdcp_conn_sem;
/* Port number for SDCP. Will be overwriten by the init function */
uint16_t CSP_SDCP_PORT = 56734;
/* Pointer to the RX function for the CSP-interface */
static csp_sdcp_rx_func csp_sdcp_rx_ref;
/* SDCP password for HMAC */
char SDCP_PASSWORD[] =  {"q7Wh7jTgXmWSb69iVF8gkiIvQmGUy9IwgrrHAebvpxuB6HU5xVz9cN75GJmqv6K"}; // 63 characters


/**
 * Initiate the SDCP driver with port number
 * @param uint16_t port (The port number the server/client uses to transmit/receive data)
 * @param csp_sdcp_rx_func rxRef (The function to be called when a new data packet is received)
 * @param char* sdcppassword (The 63 char password to use for SDCP, if NULL, default will be used)
 * @return int (-1 = error, 1 = OK)
 */
int csp_sdcp_init(uint16_t port, char * ip, csp_sdcp_rx_func rxRef, char* privatekey) {

	CSP_SDCP_PORT = port;
	csp_sdcp_rx_ref = rxRef;

	if(csp_bin_sem_create(&sdcp_conn_sem) == CSP_SEMAPHORE_ERROR) {
		csp_debug(CSP_ERROR, "No more memory for SDCP connection semaphore\r\n");
		return CSP_ERR_DRIVER;
	}

	/* Start randomizer */
	srand ( time(NULL) );

	/* Set password */
	csp_sdcp_setprivatekey(privatekey);

	/* Copy IP address to local memory before passing argument to pthreads 
	 * Note: this is needed by JNI at least. */
	static char ip_copy[16];
	strcpy(ip_copy, ip);

	/** Create incoming connection task */
	pthread_t handle_tcpip;
	int threadError;
	if((threadError = pthread_create(&handle_tcpip, NULL, csp_sdcp_newconn_task, (void *) ip_copy)) != 0) {
		csp_debug(CSP_ERROR, "Creation of TCP/IP incoming task failed: %i\r\n", threadError);
		return CSP_ERR_DRIVER;
	}

	/** Create keepalice task */
	pthread_t handle_keepalive;
	if((threadError = pthread_create(&handle_keepalive, NULL, csp_sdcp_keepaliveTask, NULL)) != 0) {
		csp_debug(CSP_ERROR, "Creation of TCP/IP incoming task failed: %i\r\n", threadError);
		return CSP_ERR_DRIVER;
	}

	/* Regsiter interface */
	csp_route_add_if(&csp_if_sia);

	return CSP_ERR_NONE;
}

/**
 * Set privatekey for SDCP driver
 *
 * @param char * privatekey (The privatekey to use. Max 64 characters incl. nul-terminator)
 */
void csp_sdcp_setprivatekey(char * privatekey) {
	if(privatekey != NULL) {
		strcpy(SDCP_PASSWORD, privatekey);
	}
}


/**
 * Transmit a CSP-packet through SDCP to IPaddress.
 * @param char * IPaddress (The destination adress for the CSP-packet in dot quatted format)
 * @param csp_packet_t * packet (A pointer to the CSP packet to be transmitted)
 * @param unsigned int timeout (The timeout in milliseconds it should wait for the connection to be free)
 * @return int (1 = OK, 0 = error)
 */
int csp_sdcp_tx(char * IPaddress, csp_packet_t * packet, unsigned int timeout) {

	csp_sdcp_conn_t * connection;
	csp_sdcp_packet_t * sdcp_packet = NULL;
	int packetLength;
	int waitcount = 0;

	/* Get a socket to send to */
	connection = csp_sdcp_findconn(IPaddress);
	if(connection == NULL) {
		csp_debug(CSP_ERROR, "Could not create connection to %s\r\n", IPaddress);
		return 0;
	}

	/* Create SDCP packet */
	packetLength = sizeof(csp_sdcp_packet_t)+packet->length;
	sdcp_packet = (csp_sdcp_packet_t *)malloc(packetLength*sizeof(char));

	csp_debug(CSP_INFO, "Sending packet to %s via SDCP\r\n", IPaddress);
	//csp_debug(CSP_INFO, "Create a packet of %i bytes with %i bytes overhead for SDCP\r\n", packetLength, sizeof(csp_sdcp_packet_t));

	// Set default timeout to 10000 ms
	if(timeout == 0) {
		timeout = 10000;
	}


	/* Make sure connection has a public key avaiable */
	while(connection->publickey == SDCP_PUBLICKEY_NON && waitcount < (timeout/10)) {
		usleep(10000); // wait 10 ms
		waitcount++;
	}
	if(connection->publickey == SDCP_PUBLICKEY_NON) {
		csp_debug(CSP_ERROR, "SDCP: Did not get a public key from %s\r\n", IPaddress);
		free(sdcp_packet);
		return 0;
	}

	/* Make sure this packet is not in a loop */
	if(memcmp(&packet->id.ext, &connection->lastpacket, 4) == 0) {
		if(connection->lastpacket_time > time(0)-CSP_SDCP_PACKETLOOP_TIMEOUT) {
			csp_debug(CSP_WARN, "SDCP: Packet %p was in a loop. Packet discarded\r\n", packet);
			free(sdcp_packet);
			return 0;
		}
	}


	/* Fill out packet */
	if(csp_bin_sem_wait(&connection->lock, timeout) == CSP_SEMAPHORE_OK) {
		if(csp_bin_sem_wait(&connection->sendlock, timeout) == CSP_SEMAPHORE_OK) {

			sdcp_packet->sequence = connection->sequence_tx;
			sdcp_packet->type = CSP_SDCP_DATA;
			sdcp_packet->length = (uint32_t)packet->length;
			memcpy(sdcp_packet->data, &packet->id.ext, packet->length);
			csp_sdcp_insert_checksum(sdcp_packet, connection);

			connection->packet_acked = 0;

			/* Send packet */
			if (csp_ip_send(connection->sock, sdcp_packet, packetLength, 0) != packetLength) {
				csp_debug(CSP_ERROR, "send() on socket %i sent a different number of bytes than expected\r\n", connection->sock);
				connection->state = CSP_SDCP_STATE_CLOSING;
				csp_debug(CSP_WARN, "Lost connection to %s\r\n", connection->ip);
				free(sdcp_packet);
				csp_bin_sem_post(&connection->lock);
				csp_bin_sem_post(&connection->sendlock);
				return 0;
			}

			// If succesed, update TX seq and unlock send lock
			connection->sequence_tx++;
			csp_bin_sem_post(&connection->sendlock);

#if SDCP_USE_ACK
			/* wait for ACK */
			// Calculate timeout
			struct timespec ts;
			if (clock_gettime(CLOCK_REALTIME, &ts)) {
				csp_debug(CSP_ERROR, "Could not get time. Send possible failed\r\n");
				csp_bin_sem_post(&connection->lock);
				free(sdcp_packet);
				return 0;
			}
			uint32_t sec = timeout / 1000;
			uint32_t nsec = (timeout - 1000 * sec) * 1000000;
			ts.tv_sec += sec;
			if (ts.tv_nsec + nsec > 1000000000)
				ts.tv_sec++;

			ts.tv_nsec = (ts.tv_nsec + nsec) % 1000000000;

			pthread_mutex_lock( &connection->sig_mutex );
			if (connection->packet_acked == 0) {
				if(pthread_cond_timedwait(&connection->signal, &connection->sig_mutex, &ts) == 0) {
					csp_debug(CSP_INFO, "SDCP packet send to %s on socket %i\r\n", IPaddress, connection->sock);
					pthread_mutex_unlock( &connection->sig_mutex );
					/* Release packet */
					free(sdcp_packet);
					csp_bin_sem_post(&connection->lock);
					return 1;
				} else {
					csp_debug(CSP_WARN, "Did not received ACK packet in time. Send failed\r\n");
					pthread_mutex_unlock( &connection->sig_mutex );
					/* Release packet */
					free(sdcp_packet);
					csp_bin_sem_post(&connection->lock);
					return 0; // Send did not get ACK
				}
			}
			pthread_mutex_unlock( &connection->sig_mutex );
#endif
			free(sdcp_packet);
			csp_bin_sem_post(&connection->lock);
			return 1;

		} else {
			csp_debug(CSP_WARN, "The SDCP send is locked. Try again\r\n");
			free(sdcp_packet);
			return 0;
		}

	} else {
		csp_debug(CSP_WARN, "The SDCP connection is locked. Try again\r\n");
		free(sdcp_packet);
		return 0;
	}

}
/**
 * Generate and insert checksum for SDCP packet via HMAC method (http://en.wikipedia.org/wiki/HMAC)
 *
 * @param csp_sdcp_packet_t * sdcp_packet (The packet to be transmitted)
 * @para, csp_packet_t * packet (The CSP packet inside sdcp_packet)
 */
void csp_sdcp_insert_checksum(csp_sdcp_packet_t * sdcp_packet, csp_sdcp_conn_t * connection) {


	int i = 0;
	char key[64];
	strcpy(key, SDCP_PASSWORD);
	char i_key_pad[64];
	char o_key_pad[64];
	int blocksize = 64; // blocksize for SHA-1


	/*
	 * 	if (length(key) > blocksize) then
     *   	key = hash(key) // keys longer than blocksize are shortened
     *	end if
	 */
	if(strlen(key) > blocksize) {
		csp_sha1_memory((uint8_t*)key, 64, (uint8_t*)key);
		key[20] = '\0';
	}

	/**
	 * 	if (length(key) < blocksize) then
     *   	key = key ∥ zeroes(blocksize - length(key)) // keys shorter than blocksize are zero-padded
     *	end if
	 */
	if(strlen(key) < blocksize) {
		for(i = strlen(key); i < 64; i++) {
			key[i] = 0;
		}
	}

	/**
	 * 	o_key_pad = [0x5c * blocksize] ⊕ key // Where blocksize is that of the underlying hash function
     *	i_key_pad = [0x36 * blocksize] ⊕ key // Where ⊕ is exclusive or (XOR)
	 */
	for(i = 0; i < blocksize; i++) {
		o_key_pad[i] = 0x5c ^ key[i];
		i_key_pad[i] = 0x36 ^ key[i];
	}


	/**
	 * return hash(o_key_pad ∥ hash(i_key_pad ∥ message)) // Where ∥ is concatenation
	 */
	// Calculate the tmp variable (midSum), for holding the data. Because of second run, the minimum size has to be blocksize+SHA-1 outsize size+ = blocksize+20
	int inputSize = blocksize+sdcp_packet->length+4+4; // Key (blocksize), message, sequence number and public key
	if(inputSize < blocksize+20) { inputSize = blocksize+20; }

	// Allocate tmp variable
	uint8_t* midSum = (uint8_t*)malloc(inputSize);

	// Insert i_key_pad into midSum
	for(i = 0; i < blocksize; i++) {
		midSum[i] = i_key_pad[i];
	}
	// Insert sequence
	midSum[blocksize] = (char)((sdcp_packet->sequence&0x000000FF) >> 0);
	midSum[blocksize+1] = (char)((sdcp_packet->sequence&0x0000FF00) >> 8);
	midSum[blocksize+2] = (char)((sdcp_packet->sequence&0x00FF0000) >> 16);
	midSum[blocksize+3] = (char)((sdcp_packet->sequence&0xFF000000) >> 24);

	// Insert public key
	midSum[blocksize+4] = (char)((connection->publickey&0x000000FF) >> 0);
	midSum[blocksize+5] = (char)((connection->publickey&0x0000FF00) >> 8);
	midSum[blocksize+6] = (char)((connection->publickey&0x00FF0000) >> 16);
	midSum[blocksize+7] = (char)((connection->publickey&0xFF000000) >> 24);

	// Insert packet content (add 8 byte offset becuase of sequence and public key)
	for(i = blocksize+8; i < blocksize+8+sdcp_packet->length; i++) {
		midSum[i] = sdcp_packet->data[i-blocksize-8];
	}



	// Calculate the SHA-1 value for midSum for all bytes
	csp_sha1_memory(midSum, blocksize+sdcp_packet->length+8, midSum);
	// Now midSum is only 20 bytes

	/* Second run for o_key_pad and previous calculated hash value */
	// Move the hash value blocksize bytes forward
	for(i = blocksize; i < blocksize+20; i++) {
		midSum[i] = midSum[i-blocksize];
	}
	// Insert the o_key_pad in the beginning
	for(i = 0; i < blocksize; i++) {
		midSum[i] = o_key_pad[i];
	}
	// And calculate the SHA-1 value
	csp_sha1_memory(midSum, blocksize+20, midSum);

	// Copy back into sdcp packet
	memcpy(sdcp_packet->checksum, midSum, 20);
	free(midSum);

}


/**
 * Find a connection to the IPaddress
 * If an open connection is found, this is returned, else a need connection is estabilished
 */
csp_sdcp_conn_t * csp_sdcp_findconn(char * IPaddress) {


	// Find a open connection to the address
	csp_sdcp_conn_t * tmp = NULL;
	if((tmp = csp_sdcp_conn_find(IPaddress)) != NULL) {
		return tmp;
	}


	// Else create a new connection to host
	return csp_sdcp_connect(IPaddress);

}

csp_sdcp_conn_t* csp_sdcp_connect(char* IP) {

	int sock;
	csp_sdcp_conn_t * tmp = NULL;

	/* Copy adress into struct */
	tmp = (csp_sdcp_conn_t * )malloc(sizeof(csp_sdcp_conn_t));
	if(tmp == NULL) {
		csp_debug(CSP_ERROR, "Failed to allocate space for connection\r\n");
		return 0;
	}
	strcpy(tmp->ip, IP);


	/* Create a reliable, stream socket using TCP */
	if ((sock = csp_ip_socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		csp_debug(CSP_ERROR, "TCP/IP socket() failed\r\n");
		free(tmp);
		return NULL;
	}


	/* Establish the connection to server */
	if (csp_ip_connect(sock, PF_INET, IP, CSP_SDCP_PORT, csp_sia_myaddress) < 0) {
		csp_debug(CSP_ERROR, "TCP/IP connect() failed while creating new connection in SDCP\r\n");
		free(tmp);
		csp_ip_close(sock);
		return NULL;
	}

	/* Bind socket, and insert into linked list */
	tmp->sock = sock;
	tmp->sequence_tx = 0;
	tmp->sequence_rx = 0;
	tmp->publickey = SDCP_PUBLICKEY_NON;
	tmp->state = CSP_SDCP_STATE_OPEN;
	// Initiate semaphore, mutex and conditions
	csp_bin_sem_create(&tmp->lock);
	csp_bin_sem_create(&tmp->sendlock);
	pthread_cond_init(&tmp->signal, NULL);
	pthread_mutex_init(&tmp->sig_mutex, NULL);

	/** Create incoming connection task */
	pthread_t handle_socket;
	int threadError;
	if((threadError = pthread_create(&handle_socket, NULL, csp_sdcp_rx_task, (void*)tmp)) != 0) {
		csp_debug(CSP_ERROR, "Creation of TCP/IP incoming task failed: %i\r\n", threadError);
		free(tmp);
		csp_ip_close(sock);
		return NULL;
	}
	pthread_detach(handle_socket);

	csp_sdcp_conn_insert(tmp);

	usleep(10000); // wait 10 ms to allow socket task to start

	return tmp;
}

/**
 * Close connection to IP
 */
void csp_sdcp_close(char * IP) {

	csp_sdcp_conn_t * tmp = NULL;

	tmp = csp_sdcp_conn_find(IP);
	if(tmp != NULL) {

		csp_sdcp_send_packet(tmp, CSP_SDCP_DISCONNECT, NULL, 0);
		tmp->state = CSP_SDCP_STATE_CLOSING;
	}
}

/**
 * Send SDCP packet on conn with type and data
 */
int csp_sdcp_send_packet(csp_sdcp_conn_t * connection, csp_sdcp_packet_type type, void * data, uint32_t length) {

	if(type == CSP_SDCP_DATA) {
		csp_debug(CSP_ERROR, "You can't transmit data packet through this function\r\n");
		return 0;
	}


	int packetLength = 0;
	csp_sdcp_packet_t * sdcp_packet;

	/* Create SDCP packet */
	if(data == NULL) {
		packetLength = sizeof(csp_sdcp_packet_t);
	} else {
		packetLength = sizeof(csp_sdcp_packet_t)+length;
	}
	sdcp_packet = (csp_sdcp_packet_t *)malloc(packetLength*sizeof(char));

	//csp_debug(CSP_INFO, "Create a packet of %i bytes with %i bytes overhead for SDCP\r\n", packetLength, sizeof(csp_sdcp_packet_t));


	if(csp_bin_sem_wait(&connection->sendlock, 500) == CSP_SEMAPHORE_OK) {
		/* Fill out packet */
		sdcp_packet->sequence = connection->sequence_tx;
		sdcp_packet->type = type;
		if(data != NULL) {
			sdcp_packet->length = (uint32_t)length;
			memcpy(sdcp_packet->data, data, length);
		} else {
			sdcp_packet->length = 0;
		}
		csp_sdcp_insert_checksum(sdcp_packet, connection);


		/* Send packet */
		if (csp_ip_send(connection->sock, sdcp_packet, packetLength, 0) != packetLength) {
			csp_debug(CSP_ERROR, "send() on socket %i sent a different number of bytes than expected\r\n", connection->sock);
			connection->state = CSP_SDCP_STATE_CLOSING;
			csp_debug(CSP_WARN, "Lost connection to %s\r\n", connection->ip);
			free(sdcp_packet);

			csp_bin_sem_post(&connection->sendlock);
			return 0;
		}
		connection->sequence_tx++;

		// Unlock send lock
		csp_bin_sem_post(&connection->sendlock);
	} else {
		csp_debug(CSP_WARN, "The SDCP send is locked. Try again\r\n");
		free(sdcp_packet);
		return 0;
	}

	/* Release packet and update connection sequence */
	free(sdcp_packet);

	return 1;



}


/**
 * Keep alive task.
 * Every 5. min sends a keep alive packet on all open connections
 * @param void* pvParameter (The input to the function, in this case NULL)
 * @return csp_thread_return_t
 */
csp_thread_return_t csp_sdcp_keepaliveTask(void * pvParameters) {

	while(1) {

		sleep(SDCP_KEEPALIVE_PERIODE);

		sem_wait(&sdcp_conn_sem);
		csp_sdcp_conn_t * tmp = sdcp_conn;
		while(tmp != NULL) {
			csp_sdcp_send_packet(tmp, CSP_SDCP_KEEPALIVE, NULL, 0);
			tmp = tmp->next;
		}
		sem_post(&sdcp_conn_sem);


	}

	return 0;
}

/** TCP/IP receive task
 * This task waits for a TCP/IP on CSP_SDCP_PORT
 */
csp_thread_return_t csp_sdcp_newconn_task(void * pvParameters) {

	csp_debug(CSP_INFO, "Starting incoming TCP/IP task on ip %s\r\n", (char *) pvParameters);

	int servSock;						/* Socket descriptor for server */
	int clntSock;						/* Socket descriptor for client */
	struct sockaddr_in ClntAddr; 		/* Client address */
	unsigned int clntLen;            	/* Length of client address data structure */
	csp_sdcp_conn_t * newConn = NULL; 	/* Pointer for new connection */


	/* Create socket for incoming conneccsp_sdcp_findconntions */
	if ((servSock = csp_ip_socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		csp_debug(CSP_ERROR, "socket() failed while creating TCP server in vCsp_sip_rxTask()\r\n");
		return 0;
	}


	/* Bind to the local address */
	while(1) {
		if (csp_ip_bind(servSock, AF_INET, CSP_SDCP_PORT, (char*) pvParameters) < 0) {
			csp_debug(CSP_WARN, "bind() to %i failed in vCsp_sip_rxTask. Waiting 2 seconds for reconnect...\r\n", CSP_SDCP_PORT);
			sleep(2);
			continue;
		}
		break;
	}

	/* Mark the socket so it will listen for incoming connections. Allow 16 connections */
	if (csp_ip_listen(servSock, 16) < 0) {
		csp_debug(CSP_ERROR, "listen() failed in vCsp_sip_rxTask()\r\n");
		return 0;
	}

	while(1) {
		/* Set the size of the in-out parameter */
		clntLen = sizeof(ClntAddr);

		/* Wait for a client to connect */
		if ((clntSock = csp_ip_accept(servSock, (struct sockaddr *) &ClntAddr, &clntLen)) < 0) {
			csp_debug(CSP_ERROR, "accept() failed in vCsp_sip_rxTask()\r\n");
			continue;
		}
		csp_debug(CSP_PACKET, "New client %s on port %i\n", inet_ntoa(ClntAddr.sin_addr), CSP_SDCP_PORT);


		/* Copy adress into struct */
		newConn = (csp_sdcp_conn_t * )malloc(sizeof(csp_sdcp_conn_t));
		strcpy(newConn->ip, inet_ntoa(ClntAddr.sin_addr));

		/* Bind socket, and insert into linked list */
		newConn->sock = clntSock;
		newConn->sequence_tx = 0;
		newConn->sequence_rx = 0;
		newConn->publickey = SDCP_PUBLICKEY_NON;
		newConn->state = CSP_SDCP_STATE_OPEN;
		// Initiate semaphore, mutex and conditions
		csp_bin_sem_create(&newConn->lock);
		csp_bin_sem_create(&newConn->sendlock);
		pthread_cond_init(&newConn->signal, NULL);
		pthread_mutex_init(&newConn->sig_mutex, NULL);

		/* Generate public key and send. */
		uint32_t publickey = rand()+1; // Todo: Fix this so it includes to entire 32 bit range
		csp_sdcp_send_packet(newConn, CSP_SDCP_KEYEXCHANGE, &publickey, 4);
		newConn->publickey = publickey;

		/** Create incoming connection task */
		pthread_t handle_socket;
		int threadError;
		if((threadError = pthread_create(&handle_socket, NULL, csp_sdcp_rx_task, (void*)newConn)) != 0) {
			csp_debug(CSP_ERROR, "Creation of TCP/IP incoming task failed: %i\r\n", threadError);
			csp_ip_close(clntSock);
			free(newConn);
			continue;
		}
		pthread_detach(handle_socket);

		csp_sdcp_conn_insert(newConn);

	}

}
/**
 * Check the checksum of received packet
 * Pulls out the checksum in the packet, clear the checksum in the packet and generate a new, and compare the two checksums
 *
 * @param csp_sdcp_packet_t * sdcp_packet (The SDCP packet to validate)
 * @param csp_sdcp_conn_t * conn (The connection on which the SDCP packet was received)
 * @return int (0 = error, 1 = OK)
 */
static inline int csp_sdcp_rx_checksum(csp_sdcp_packet_t * sdcp_packet, csp_sdcp_conn_t * conn) {
	// Variables
	unsigned char orgChecksum[sizeof(sdcp_packet->checksum)];


	memcpy(&orgChecksum, sdcp_packet->checksum, sizeof(sdcp_packet->checksum));
	// Clear the checsum
	bzero(sdcp_packet->checksum, sizeof(sdcp_packet->checksum));
	// Insert new packet from packet content
	csp_sdcp_insert_checksum(sdcp_packet, conn);


	if(strncmp((char*)orgChecksum, (char*)sdcp_packet->checksum, sizeof(sdcp_packet->checksum)) != 0) {
		csp_debug(CSP_ERROR, "SDCP: Checksum error for packet (s: %u)\r\n", sdcp_packet->sequence);
		if(sdcp_packet->type != CSP_SDCP_CHECKERROR) {
			csp_sdcp_send_packet(conn, CSP_SDCP_CHECKERROR, NULL, 0);
		}
		return 0;
	}
	return 1;
}
/**
 * Check the SDCP sequence number with the connection sequence number
 * Also handles if the received packet is a SYNC packet
 *
 * @param csp_sdcp_packet_t * sdcp_packet (The SDCP packet to validate)
 * @param csp_sdcp_conn_t * conn (The connection on which the SDCP packet was received)
 * @return (0 = Error, 1 = OK)
 */
static inline int csp_sdcp_rx_sequence(csp_sdcp_packet_t * sdcp_packet, csp_sdcp_conn_t * conn) {
	if(sdcp_packet->type == CSP_SDCP_SEQSYNC) {
		csp_debug(CSP_ERROR, "Sequence error for last packet on socket %i\r\n", conn->sock);
		uint32_t newSeq = (uint32_t)*sdcp_packet->data;

		if((newSeq > conn->sequence_tx) || (newSeq < 10)) {
			sem_wait(&conn->lock);
			conn->sequence_tx = newSeq;
			sem_post(&conn->lock);
			csp_debug(CSP_WARN, "Sync sequence number for socket %i to %u\r\n", conn->sock, conn->sequence_tx);
		} else {
			csp_debug(CSP_ERROR, "Received sync request for SDCP connection %i, but sequence not uniq\r\n", conn->sock, conn->sequence_tx);
		}
		return 0;
	}

	// If the packets sequence number allready has been used, reject and sync
	if(conn->sequence_rx > sdcp_packet->sequence) {
		csp_debug(CSP_WARN, "SDCP: Sequence error for packet (s: %u). Synchronizing...\r\n", sdcp_packet->sequence);
		// Synchronise sequence numbers
		if(sdcp_packet->type != CSP_SDCP_SEQSYNC) {

			conn->sequence_rx += 10;
			csp_sdcp_send_packet(conn, CSP_SDCP_SEQSYNC, (void *)(&conn->sequence_rx), 4);

		}
		return 0;
	}
	return 1;
}

/**
 * Thread for each open connection
 *
 * @param void * pvParameters (A pointer to the conn which the thread is monitoring)
 * @return csp_thread_return_t
 */
csp_thread_return_t csp_sdcp_rx_task(void * pvParameters) {

	csp_sdcp_conn_t * conn = (csp_sdcp_conn_t *)pvParameters;
	csp_sdcp_packet_t * sdcp_packet;		/* Pointer to the received packet */
	csp_packet_t * packet;
	int recvMsgSize;                    /* Size of received message */

	csp_debug(CSP_INFO, "Started socket task for socket %i\r\n", conn->sock);

	sdcp_packet = malloc(1000);
	while(conn->state) {

		/* Receive header from client */
		if ((recvMsgSize = csp_ip_recv(conn->sock, sdcp_packet, sizeof(csp_sdcp_packet_t), 0)) < 0) {
			csp_debug(CSP_ERROR, "recv() failed\r\n");
			break;
		}
		if(recvMsgSize == 0) {
			break;
		}
		/* Check if received packet is minimum length */
		if(recvMsgSize < sizeof(csp_sdcp_packet_t)) {
			csp_debug(CSP_WARN, "Socket %i received packet of wrong length: %i\r\n", conn->sock, recvMsgSize);
			continue;
		}

		/* Get payload of packet */
		if(sdcp_packet->length > 0) {
			if ((recvMsgSize = csp_ip_recv(conn->sock, sdcp_packet->data, sdcp_packet->length, 0)) < 0) {
				csp_debug(CSP_ERROR, "recv() failed while getting content of packet\r\n");
				break;
			}
		}


		/*
		 * Sequence number check and sync
		 */
		if(csp_sdcp_rx_sequence(sdcp_packet, conn) == 0) {
			continue;
		}
		// Sequence number checked out, continue

		/*
		 * Check checksum, by copying the received checksum, generate new, and compare
		 */
		if(csp_sdcp_rx_checksum(sdcp_packet, conn) == 0) {
			continue;
		}

		conn->sequence_rx++;

		/*
		 * Handle the packet
		 */

		if(conn->publickey == SDCP_PUBLICKEY_NON && sdcp_packet->type == CSP_SDCP_DATA) {
			csp_debug(CSP_ERROR, "SDCP: No public key set for socket %i to %s\r\n", conn->sock, conn->ip);
			continue;
		}


		switch(sdcp_packet->type) {
			case CSP_SDCP_DATA:
#if SDCP_USE_ACK
				csp_sdcp_send_packet(conn, CSP_SDCP_ACK, NULL, 0);
#endif

				/* Allocate and fill CSP packet */
				packet = csp_buffer_get(200);
				memcpy(&packet->id.ext, sdcp_packet->data, sdcp_packet->length);

				/* Save a pointer to the last packet. */
				memcpy(&conn->lastpacket, &packet->id.ext, 4);
				conn->lastpacket_time = time(0);

				/* Set the packet length of received packet */
				packet->length = sdcp_packet->length;
				csp_sdcp_rx_ref(packet);

				break;
			case CSP_SDCP_ACK:
				pthread_mutex_lock( &conn->sig_mutex );
				conn->packet_acked = 1;
				pthread_cond_signal( &conn->signal );
				pthread_mutex_unlock( &conn->sig_mutex );
				break;
			case CSP_SDCP_KEYEXCHANGE:
				if(sdcp_packet->sequence == 0 && conn->publickey == 0x0) {
					sem_wait(&conn->sendlock);
					memcpy(&conn->publickey, sdcp_packet->data, 4);
					sem_post(&conn->sendlock);
					csp_debug(CSP_INFO, "Received public key 0x%X for socket %i\r\n", conn->publickey, conn->sock);
				} else {
					csp_debug(CSP_WARN, "Received public key, but only sequencenum. 0 can be key exchange\r\n");
				}
				break;
			case CSP_SDCP_KEEPALIVE:
					csp_debug(CSP_INFO, "Keep-alive request on socket %i to %s\r\n", conn->sock, conn->ip);
				break;
			case CSP_SDCP_DISCONNECT:
					conn->state = CSP_SDCP_STATE_CLOSING;
				break;
			case CSP_SDCP_CHECKERROR:
					csp_debug(CSP_ERROR, "SDCP packet (s: %i) not delivered: Checksum error\r\n", sdcp_packet->sequence);
					sem_wait(&conn->sendlock);
					conn->sequence_tx--;
					sem_post(&conn->sendlock);
				break;
			default:
				csp_debug(CSP_WARN, "Received unknown packet type: %i\r\n", sdcp_packet->type);
				break;
		}




	}

	/**
	 * Release connection
	 */
	csp_debug(CSP_INFO, "Waiting on getting lock for connection to close\r\n");
	sem_wait(&conn->sendlock);


	csp_debug(CSP_INFO, "Closing connection on socket %i to %s\r\n", conn->sock, conn->ip);

	/* Release the connection from the list */
	csp_sdcp_conn_remove(conn);

	free(sdcp_packet);
	csp_ip_close(conn->sock);
	free(conn);
	return 0;
}


/**
 * Insert a new connection in connection list
 *
 * @param csp_sdcp_conn_t * newNode (The entry which to insert)
 * @return vodi
 */
void csp_sdcp_conn_insert(csp_sdcp_conn_t * newNode) {
	sem_wait(&sdcp_conn_sem);
	newNode->next = sdcp_conn;
	sdcp_conn = newNode;
	sem_post(&sdcp_conn_sem);
}
/**
 * Remove a node from the linked list
 *
 * @param csp_sdcp_conn_t * conn (The entry to remove
 */
void csp_sdcp_conn_remove(csp_sdcp_conn_t * conn) {
	sem_wait(&conn->lock); // Lock the connection, never unlock because connections is removed
	sem_wait(&sdcp_conn_sem);
	csp_sdcp_conn_t * tmp = sdcp_conn;
	if(tmp == conn) {
		sdcp_conn = conn->next;
	} else {
		while(tmp->next != NULL) {
			if(tmp->next == conn) {
				tmp->next = conn->next;
				break;
			}
			tmp = tmp->next;
		}
	}
	sem_post(&sdcp_conn_sem);
}

/**
 * Find a connection entry connected to IPaddress
 *
 * @param char * IPaddress
 * @return csp_sdcp_conn_t * (The connection OR NULL)
 */
csp_sdcp_conn_t * csp_sdcp_conn_find(char * IPaddress) {
	// Find a open connection to the address
	csp_sdcp_conn_t * tmp = sdcp_conn;
	tmp = sdcp_conn;
	while(tmp != NULL) {

		if(tmp->ip != NULL) {
			if(strncmp(tmp->ip, IPaddress, strlen(IPaddress)) == 0) {
				return tmp;
			}
		}

		tmp = tmp->next;
	}
	return NULL;
}


/*****************************************************************************************************************************
 *****************************************************************************************************************************
 ** 													END OF SDCP															**
 *****************************************************************************************************************************
 *****************************************************************************************************************************/
