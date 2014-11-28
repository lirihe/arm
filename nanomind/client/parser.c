#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <malloc.h>
#include <util/byteorder.h>

void hex_dump(void *src, int len) {
	int i, j=0, k;
	char text[17];

	text[16] = '\0';
	//printf("Hex dump:\r\n");
	printf("%p : ", src);
	for(i=0; i<len; i++) {
		j++;
		printf("%02X ",((volatile unsigned char *)src)[i]);
		if(j == 8)
			printf(" ");
		if(j == 16) {
			j = 0;
			memcpy(text, &((char *)src)[i-15], 16);
			for(k=0; k<16; k++) {
				if((text[k] < 32) || (text[k] > 126)) {
					text[k] = '.';
				}
			}
			printf(" |");
			printf("%s", text);
			printf("|\n\r");
			if(i<len-1) {
				printf("%p : ", src+i+1);
			}
		}
	}
	if (i % 16)
		printf("\r\n");
}

/* Convert 16-bit number from host byte order to network byte order */
uint16_t __attribute__ ((__const__)) csp_hton16(uint16_t h16) {
#ifdef CSP_BIG_ENDIAN
	return h16;
#else
	return (((h16 & 0xff00) >> 8) |
			((h16 & 0x00ff) << 8));
#endif
}

/* Convert 16-bit number from network byte order to host byte order */
uint16_t __attribute__ ((__const__)) csp_ntoh16(uint16_t n16) {
	return csp_hton16(n16);
}

/* Convert 32-bit number from host byte order to network byte order */
uint32_t __attribute__ ((__const__)) csp_hton32(uint32_t h32) {
#ifdef CSP_BIG_ENDIAN
	return h32;
#else
	return (((h32 & 0xff000000) >> 24) |
			((h32 & 0x000000ff) << 24) |
			((h32 & 0x0000ff00) <<  8) |
			((h32 & 0x00ff0000) >>  8));
#endif
}

/* Convert 32-bit number from network byte order to host byte order */
uint32_t __attribute__ ((__const__)) csp_ntoh32(uint32_t n32) {
	return csp_hton32(n32);
}

/* Convert 64-bit number from host byte order to network byte order */
uint64_t __attribute__ ((__const__)) csp_hton64(uint64_t h64) {
#ifdef CSP_BIG_ENDIAN
	return h64;
#else
	return (((h64 & 0xff00000000000000LL) >> 56) |
			((h64 & 0x00000000000000ffLL) << 56) |
			((h64 & 0x00ff000000000000LL) >> 40) |
			((h64 & 0x000000000000ff00LL) << 40) |
			((h64 & 0x0000ff0000000000LL) >> 24) |
			((h64 & 0x0000000000ff0000LL) << 24) |
			((h64 & 0x000000ff00000000LL) >>  8) |
			((h64 & 0x00000000ff000000LL) <<  8));
#endif
}

/* Convert 64-bit number from host byte order to network byte order */
uint64_t __attribute__ ((__const__)) csp_ntoh64(uint64_t n64) {
	return csp_hton64(n64);
}

/* Convert 16-bit number from host byte order to big endian byte order */
uint16_t __attribute__ ((__const__)) csp_htobe16(uint16_t h16) {
	return csp_hton16(h16);
}

/* Convert 16-bit number from host byte order to little endian byte order */
uint16_t __attribute__ ((__const__)) csp_htole16(uint16_t h16) {
#ifdef CSP_LITTLE_ENDIAN
	return h16;
#else
	return (((h16 & 0xff00) >> 8) |
			((h16 & 0x00ff) << 8));
#endif
}

/* Convert 16-bit number from big endian byte order to little endian byte order */
uint16_t __attribute__ ((__const__)) csp_betoh16(uint16_t be16) {
	return csp_ntoh16(be16);
}

/* Convert 16-bit number from little endian byte order to host byte order */
uint16_t __attribute__ ((__const__)) csp_letoh16(uint16_t le16) {
	return csp_htole16(le16);
}

/* Convert 32-bit number from host byte order to big endian byte order */
uint32_t __attribute__ ((__const__)) csp_htobe32(uint32_t h32) {
	return csp_hton32(h32);
}

/* Convert 32-bit number from little endian byte order to host byte order */
uint32_t __attribute__ ((__const__)) csp_htole32(uint32_t h32) {
#ifdef CSP_LITTLE_ENDIAN
	return h32;
#else
	return (((h32 & 0xff000000) >> 24) |
			((h32 & 0x000000ff) << 24) |
			((h32 & 0x0000ff00) <<  8) |
			((h32 & 0x00ff0000) >>  8));
#endif
}

/* Convert 32-bit number from big endian byte order to host byte order */
uint32_t __attribute__ ((__const__)) csp_betoh32(uint32_t be32) {
	return csp_ntoh32(be32);
}

/* Convert 32-bit number from little endian byte order to host byte order */
uint32_t __attribute__ ((__const__)) csp_letoh32(uint32_t le32) {
	return csp_htole32(le32);
}

/* Convert 64-bit number from host byte order to big endian byte order */
uint64_t __attribute__ ((__const__)) csp_htobe64(uint64_t h64) {
	return csp_hton64(h64);
}

/* Convert 64-bit number from host byte order to little endian byte order */
uint64_t __attribute__ ((__const__)) csp_htole64(uint64_t h64) {
#ifdef CSP_LITTLE_ENDIAN
	return h64;
#else
	return (((h64 & 0xff00000000000000LL) >> 56) |
			((h64 & 0x00000000000000ffLL) << 56) |
			((h64 & 0x00ff000000000000LL) >> 40) |
			((h64 & 0x000000000000ff00LL) << 40) |
			((h64 & 0x0000ff0000000000LL) >> 24) |
			((h64 & 0x0000000000ff0000LL) << 24) |
			((h64 & 0x000000ff00000000LL) >>  8) |
			((h64 & 0x00000000ff000000LL) <<  8));
#endif
}

/* Convert 64-bit number from big endian byte order to host byte order */
uint64_t __attribute__ ((__const__)) csp_betoh64(uint64_t be64) {
	return csp_ntoh64(be64);
}

/* Convert 64-bit number from little endian byte order to host byte order */
uint64_t __attribute__ ((__const__)) csp_letoh64(uint64_t le64) {
	return csp_htole64(le64);
}

/* BEACON A */
struct __attribute__((__packed__)) cdh_beacon_a {
	struct __attribute__((packed)) {
		uint16_t bootcount;				// Total boot count
		int16_t temp1;					// Board temp1 * 4 in [C]
		int16_t temp2;					// Board temp2 * 4 in [C]
		int16_t  panel_temp[6];			// Panel temperatures * 4 in [C]
	} obc;
	struct __attribute__((packed)) {
		uint16_t byte_corr_tot;			// Total bytes corrected by reed-solomon
		uint16_t rx;					// Total packets detected
		uint16_t rx_err;				// Total packets with error
		uint16_t tx;					// Total packets transmitted
		int16_t  last_temp_a;			// Last temperature A in [C]
		int16_t  last_temp_b;			// Last temperature B in [C]
		int16_t  last_rssi;				// Last detected RSSI [dBm]
		int16_t  last_rferr;			// Last detected RF-error [Hz]
		uint16_t last_batt_volt;		// Last sampled battery voltage [mV/10]
		uint16_t last_txcurrent;		// Last TX current [mA]
		uint16_t bootcount;				// Total bootcount
	} com;
	struct __attribute__ ((__packed__)) {
		uint16_t vboost[3];								//! Voltage of boost converters [mV] [PV1, PV2, PV3]
		uint16_t vbatt;									//! Voltage of battery [mV]
		uint16_t curout[6];								//! Current out [mA]
		uint16_t curin[3];								//! Current in [mA]
		uint16_t cursun;								//! Current from boost converters
		uint16_t cursys;								//! Current out of battery
		int16_t temp[6];								//! Temperature sensors [0 = TEMP1, TEMP2, TEMP3, TEMP4, BATT0, BATT1]
		uint8_t output;									//! Status of outputs
		uint16_t counter_boot; 							//! Number of EPS reboots
		uint16_t counter_wdt_i2c;						//! Number of WDT I2C reboots
		uint16_t counter_wdt_gnd;						//! Number of WDT GND reboots
		uint8_t	bootcause;								//! Cause of last EPS reset
		uint16_t latchup[6];							//! Number of latch-ups
		uint8_t battmode;								//! Mode for battery [0 = normal, 1 = undervoltage, 2 = overvoltage]
	} eps;
	struct __attribute__((__packed__)) {
		uint16_t average_fps_5min;
		uint16_t average_fps_1min;
		uint16_t average_fps_10sec;
		uint16_t plane_count;
		uint32_t frame_count;
		uint32_t last_icao;
		uint32_t last_timestamp;
		float last_lat;
		float last_lon;
		uint32_t last_altitude;
		uint32_t crc_corrected;
		uint16_t bootcount;
		uint16_t bootcause;
	} gatoss;
	struct __attribute__((packed)) {
		int8_t  temp;					// Temperature of nanohub in [C]
		uint16_t bootcount;				// Total bootcount
		uint8_t  reset;					// Reset cause:
		uint8_t  sense_status;			// Status on feedback switches and arm switch [ARM1 ARM0 K1S3 K1S2 K1S1 K1S0 K0S1 K0S0]
		uint16_t burns[2];				// Number of burn tries [K1B1 K1B0]
	} hub;
	struct __attribute__((packed)) {
		float tumblerate[3];
		float tumblenorm[2];
		float mag[3];
		uint8_t status;					// [xxxxxxab] a = magvalid, b = detumbled
		float torquerduty[3];
		uint8_t ads;					// State [xxxxyyyy] x = state, y = dstate
		uint8_t acs;					// State [xxxxyyyy] x = state, y = dstate
	    uint8_t sunsensor_packed[8];
	} adcs;
};

/* BEACON AB */
struct __attribute__((__packed__)) cdh_beacon_b {
	    uint16_t sun[5]; 		// sun sensor +x +y -x -y -z
	    uint8_t ineclipse;		// 0=in sun, 1=ineclipse
	    float xest[16]; 		// State vector [_i^cq ^c\omega b_mag b_gyro \tau] in total 16 states
	    float zfilt[9]; 		// Filt meas vector [v_sun v_mag omega_gyro]
	    uint16_t enable; 		// Enable vector (bit mask) for meas vector [xsunsensor,3xmag,3xgyro]
	    float ref_q[4];			// Control ref
		float err_q[4];			// control err
	    float ierr_q[3];		// Control ierr
	    float err_rate[3];		// Control err rate
	    float  sc_reci[3];		// Ephem satellite pos
	    float  sun_eci[3];		// Ephem sun pos
	    float  mag_eci[3];		// Ephem mag
};

/* BEACON */
typedef struct __attribute__((packed)) {
	uint32_t 	beacon_time;							//! Time the beacon was formed
	uint8_t 	beacon_flags;							//! Which type of beacon is this
	union {
		struct cdh_beacon_a a;							//! A is common housekeeping
		struct cdh_beacon_b b;							//! B is extended ADCS
	};
} cdh_beacon;


static void cdh_beacon_ntoh(cdh_beacon * beacon) {
	beacon->beacon_time = csp_ntoh32(beacon->beacon_time);

	beacon->a.obc.bootcount = csp_ntoh16(beacon->a.obc.bootcount);
	beacon->a.obc.temp1 = csp_ntoh16(beacon->a.obc.temp1);
	beacon->a.obc.temp2 = csp_ntoh16(beacon->a.obc.temp2);
	beacon->a.obc.panel_temp[0] = csp_ntoh16(beacon->a.obc.panel_temp[0]);
	beacon->a.obc.panel_temp[1] = csp_ntoh16(beacon->a.obc.panel_temp[1]);
	beacon->a.obc.panel_temp[2] = csp_ntoh16(beacon->a.obc.panel_temp[2]);
	beacon->a.obc.panel_temp[3] = csp_ntoh16(beacon->a.obc.panel_temp[3]);
	beacon->a.obc.panel_temp[4] = csp_ntoh16(beacon->a.obc.panel_temp[4]);
	beacon->a.obc.panel_temp[5] = csp_ntoh16(beacon->a.obc.panel_temp[5]);

	beacon->a.com.byte_corr_tot = csp_ntoh16(beacon->a.com.byte_corr_tot);
	beacon->a.com.rx = csp_ntoh16(beacon->a.com.rx);
	beacon->a.com.rx_err = csp_ntoh16(beacon->a.com.rx_err);
	beacon->a.com.tx = csp_ntoh16(beacon->a.com.tx);
	beacon->a.com.last_temp_a = csp_ntoh16(beacon->a.com.last_temp_a);
	beacon->a.com.last_temp_b = csp_ntoh16(beacon->a.com.last_temp_b);
	beacon->a.com.last_rssi = csp_ntoh16(beacon->a.com.last_rssi);
	beacon->a.com.last_rferr = csp_ntoh16(beacon->a.com.last_rferr);
	beacon->a.com.last_batt_volt = csp_ntoh16(beacon->a.com.last_batt_volt);
	beacon->a.com.last_txcurrent = csp_ntoh16(beacon->a.com.last_txcurrent);
	beacon->a.com.bootcount = csp_ntoh16(beacon->a.com.bootcount);

	for (int i = 0; i < 3; i++)
		beacon->a.eps.vboost[i] = csp_ntoh16(beacon->a.eps.vboost[i]);
	beacon->a.eps.vbatt = csp_ntoh16(beacon->a.eps.vbatt);
	for (int i = 0; i < 6; i++)
		beacon->a.eps.curout[i] = csp_ntoh16(beacon->a.eps.curout[i]);
	for (int i = 0; i < 3; i++)
		beacon->a.eps.curin[i] = csp_ntoh16(beacon->a.eps.curin[i]);
	beacon->a.eps.cursun = csp_ntoh16(beacon->a.eps.cursun);
	beacon->a.eps.cursys = csp_ntoh16(beacon->a.eps.cursys);
	for (int i = 0; i < 6; i++)
		beacon->a.eps.temp[i] = csp_ntoh16(beacon->a.eps.temp[i]);
	beacon->a.eps.counter_boot = csp_ntoh16(beacon->a.eps.counter_boot);
	beacon->a.eps.counter_wdt_i2c = csp_ntoh16(beacon->a.eps.counter_wdt_i2c);
	beacon->a.eps.counter_wdt_gnd = csp_ntoh16(beacon->a.eps.counter_wdt_gnd);
	beacon->a.eps.bootcause = beacon->a.eps.bootcause;
	for (int i = 0; i < 6; i++)
		beacon->a.eps.latchup[i] = csp_ntoh32(beacon->a.eps.latchup[i]);
	beacon->a.eps.battmode = beacon->a.eps.battmode;

	beacon->a.gatoss.average_fps_5min = csp_ntoh16(beacon->a.gatoss.average_fps_5min);
	beacon->a.gatoss.average_fps_1min = csp_ntoh16(beacon->a.gatoss.average_fps_1min);
	beacon->a.gatoss.average_fps_10sec = csp_ntoh16(beacon->a.gatoss.average_fps_10sec);
	beacon->a.gatoss.plane_count = csp_ntoh16(beacon->a.gatoss.plane_count);
	beacon->a.gatoss.frame_count = csp_ntoh32(beacon->a.gatoss.frame_count);
	beacon->a.gatoss.last_icao = csp_ntoh32(beacon->a.gatoss.last_icao);
	beacon->a.gatoss.last_altitude = csp_ntoh32(beacon->a.gatoss.last_altitude);
	beacon->a.gatoss.last_timestamp = csp_ntoh32(beacon->a.gatoss.last_timestamp);
	beacon->a.gatoss.last_lat = util_ntohflt(beacon->a.gatoss.last_lat);
	beacon->a.gatoss.last_lon = util_ntohflt(beacon->a.gatoss.last_lon);
	beacon->a.gatoss.crc_corrected = csp_ntoh32(beacon->a.gatoss.crc_corrected);
	beacon->a.gatoss.bootcount = csp_ntoh16(beacon->a.gatoss.bootcount);
	beacon->a.gatoss.bootcause = csp_ntoh16(beacon->a.gatoss.bootcause);

	beacon->a.hub.bootcount = csp_ntoh16(beacon->a.hub.bootcount);
	beacon->a.hub.burns[0] = csp_ntoh16(beacon->a.hub.burns[0]);
	beacon->a.hub.burns[1] = csp_ntoh16(beacon->a.hub.burns[1]);

	beacon->a.adcs.mag[0] = util_htonflt(beacon->a.adcs.mag[0]);
	beacon->a.adcs.mag[1] = util_htonflt(beacon->a.adcs.mag[1]);
	beacon->a.adcs.mag[2] = util_htonflt(beacon->a.adcs.mag[2]);
	beacon->a.adcs.tumblenorm[0] = util_htonflt(beacon->a.adcs.tumblenorm[0]);
	beacon->a.adcs.tumblenorm[1] = util_htonflt(beacon->a.adcs.tumblenorm[1]);
	beacon->a.adcs.tumblerate[0] = util_htonflt(beacon->a.adcs.tumblerate[0]);
	beacon->a.adcs.tumblerate[1] = util_htonflt(beacon->a.adcs.tumblerate[1]);
	beacon->a.adcs.tumblerate[2] = util_htonflt(beacon->a.adcs.tumblerate[2]);
	beacon->a.adcs.torquerduty[0] = util_htonflt(beacon->a.adcs.torquerduty[0]);
	beacon->a.adcs.torquerduty[1] = util_htonflt(beacon->a.adcs.torquerduty[1]);
	beacon->a.adcs.torquerduty[2] = util_htonflt(beacon->a.adcs.torquerduty[2]);

}

void cdh_beacon_print(cdh_beacon * beacon) {
printf("\r\n");
printf("***********************\r\n");
printf("*       BEACON        *\r\n");
printf("***********************\r\n\r\n");

printf("Time: %"PRIu32", mask: %"PRIx8"\r\n", beacon->beacon_time, beacon->beacon_flags);

/* Beacon A */
//if (beacon->beacon_flags & (1 << 0)) {

	printf("\r\nOBC:\r\n");
	printf("Temp board: [%.1f, %.1f], Panels: [%.1f %.1f %.1f %.1f %.1f %.1f], Boot count: [%"PRIu16"]\r\n",
			beacon->a.obc.temp1 / 4.0,
			beacon->a.obc.temp2 / 4.0,
			beacon->a.obc.panel_temp[0] / 4.0,
			beacon->a.obc.panel_temp[1] / 4.0,
			beacon->a.obc.panel_temp[2] / 4.0,
			beacon->a.obc.panel_temp[3] / 4.0,
			beacon->a.obc.panel_temp[4] / 4.0,
			beacon->a.obc.panel_temp[5] / 4.0,
			beacon->a.obc.bootcount);

	printf("\r\nCOM:\r\n");
	printf("RX: %"PRIu32" ok, %"PRIu32" err with [%"PRIu32" byte] corrected\r\n",
			beacon->a.com.rx,
			beacon->a.com.rx_err,
			beacon->a.com.byte_corr_tot);
	printf("TX: %"PRIu32"\r\n",
			beacon->a.com.tx);
	printf("RSSI: %"PRId16", RFerr: %"PRId16", Temp A: %"PRId16", B: %"PRId16", Vbat: %"PRIu16", TXcur: %"PRIu16", Boots: %"PRIu16"\r\n",
			beacon->a.com.last_rssi,
			beacon->a.com.last_rferr,
			beacon->a.com.last_temp_a,
			beacon->a.com.last_temp_b,
			beacon->a.com.last_batt_volt,
			beacon->a.com.last_txcurrent,
			beacon->a.com.bootcount);

	printf("\r\nEPS:\r\n");
	printf("Vbat: %"PRIu16"mV, In: %"PRIu16"mA, Out %"PRIu16"mA\r\n",
			beacon->a.eps.vbatt,
			beacon->a.eps.cursun,
			beacon->a.eps.cursys);
	printf("Panel voltage: [%"PRIu16"mV, %"PRIu16"mV, %"PRIu16"mV]\r\n",
			beacon->a.eps.vboost[0],
			beacon->a.eps.vboost[1],
			beacon->a.eps.vboost[2]);
	printf("Temp: [C1: %"PRId16", C2: %"PRId16", C3: %"PRId16", Board: %"PRId16", Batt1: %"PRId16", Batt2: %"PRId16"]\r\n",
			beacon->a.eps.temp[0],
			beacon->a.eps.temp[1],
			beacon->a.eps.temp[2],
			beacon->a.eps.temp[3],
			beacon->a.eps.temp[4],
			beacon->a.eps.temp[5]);
	printf("Latchup: [%"PRId16", %"PRId16", %"PRId16", %"PRId16", %"PRId16", %"PRId16"]\r\n",
			beacon->a.eps.latchup[0],
			beacon->a.eps.latchup[1],
			beacon->a.eps.latchup[2],
			beacon->a.eps.latchup[3],
			beacon->a.eps.latchup[4],
			beacon->a.eps.latchup[5]);
	printf("boot count: %"PRIu16", cause: %"PRIu8"\r\n",
			beacon->a.eps.counter_boot,
			beacon->a.eps.bootcause);
	printf("Outputs: %"PRIx8"\r\n", beacon->a.eps.output);

	if ((beacon->beacon_flags & 0x80) > 0) {
		printf("\r\nGATOSS:\r\n");
		printf("Last position: ICAO %"PRIx32", lat: %f, lon: %f, alt: %"PRIu32", time: %"PRIu32"\r\n",
				beacon->a.gatoss.last_icao,
				beacon->a.gatoss.last_lat,
				beacon->a.gatoss.last_lon,
				beacon->a.gatoss.last_altitude,
				beacon->a.gatoss.last_timestamp);
		printf("FPS: %"PRIu16", %"PRIu16", %"PRIu16" , planes: %"PRIu16", frames: %"PRIu32", crc-corrected: %"PRIu32"\r\n",
				beacon->a.gatoss.average_fps_5min,
				beacon->a.gatoss.average_fps_1min,
				beacon->a.gatoss.average_fps_10sec,
				beacon->a.gatoss.plane_count,
				beacon->a.gatoss.frame_count,
				beacon->a.gatoss.crc_corrected);
		printf("boot count: %"PRIu16", cause: %"PRIu16"\r\n",
				beacon->a.gatoss.bootcount,
				beacon->a.gatoss.bootcause);
	}

	printf("\r\nHUB:\r\n");
	printf("temp: %"PRIi8", bootcount: %"PRIu16", bootcause: %"PRIu8"\r\n",
			beacon->a.hub.temp,
			beacon->a.hub.bootcount,
			beacon->a.hub.reset);
	printf("Status Sense 0x%"PRIx8"\r\n",
			beacon->a.hub.sense_status);
	printf("Burns [K1B1: %"PRIu16", K1B0: %"PRIu16"]\r\n",
			beacon->a.hub.burns[0],
			beacon->a.hub.burns[1]);

	printf("\r\nADCS:\r\n");
	printf("Detumbled: %"PRIu8"\r\n", beacon->a.adcs.status & 1);
	printf("Mag [0]: %f, [1]: %f, [2]: %f\r\n", beacon->a.adcs.mag[0], beacon->a.adcs.mag[1], beacon->a.adcs.mag[2]);
	printf("Magvalid: %"PRIu8"\r\n", (beacon->a.adcs.status >> 1) & 1);
	printf("Torquerduty [0]: %f, [1]: %f, [2]: %f\r\n", beacon->a.adcs.torquerduty[0], beacon->a.adcs.torquerduty[1], beacon->a.adcs.torquerduty[2]);
	printf("Tumblenorm [0]: %f, [1]: %f\r\n", beacon->a.adcs.tumblenorm[0], beacon->a.adcs.tumblenorm[1]);
	printf("Tumblerate [0]: %f, [1]: %f, [2]: %f\r\n", beacon->a.adcs.tumblerate[0], beacon->a.adcs.tumblerate[1], beacon->a.adcs.tumblerate[2]);
	printf("ADS state %u desired %u\r\n", beacon->a.adcs.ads >> 4, beacon->a.adcs.ads & 0xf);
	printf("ACS state %u desired %u\r\n", beacon->a.adcs.acs >> 4, beacon->a.adcs.acs & 0xf);

	/* Unpack sunsensors */
	uint16_t sunsensor[5];
	sunsensor[0] = beacon->a.adcs.sunsensor_packed[5] & 0xf;
	sunsensor[0] = (sunsensor[0] << 8) | beacon->a.adcs.sunsensor_packed[0];
	sunsensor[1] = (beacon->a.adcs.sunsensor_packed[5] >> 4) & 0xf;
	sunsensor[1] = (sunsensor[1] << 8) | beacon->a.adcs.sunsensor_packed[1];
	sunsensor[2] = beacon->a.adcs.sunsensor_packed[6] & 0xf;
	sunsensor[2] = (sunsensor[2] << 8) | beacon->a.adcs.sunsensor_packed[2];
	sunsensor[3] = (beacon->a.adcs.sunsensor_packed[6] >> 4) & 0xf;
	sunsensor[3] = (sunsensor[3] << 8) | beacon->a.adcs.sunsensor_packed[3];
	sunsensor[4] = beacon->a.adcs.sunsensor_packed[7] & 0xf;
	sunsensor[4] = (sunsensor[4] << 8) | beacon->a.adcs.sunsensor_packed[4];

	printf("Sun sensor [+x +y -x -y -z]: %"PRIu16" %"PRIu16" %"PRIu16" %"PRIu16" %"PRIu16"\r\n",
			sunsensor[0], sunsensor[1], sunsensor[2], sunsensor[3],	sunsensor[4]);



}


void base16_encode(uint8_t *raw, size_t len, char *encoded) {
	uint8_t *raw_bytes = raw;
	char *encoded_bytes = encoded;
	size_t remaining = len;

	for (; remaining--; encoded_bytes += 2)
		snprintf(encoded_bytes, 3, "%02X", *(raw_bytes++));

}

int base16_decode(const char *encoded, uint8_t *raw) {
	const char *encoded_bytes = encoded;
	uint8_t *raw_bytes = raw;
	char buf[3];
	char *endp;
	size_t len;

	while (encoded_bytes[0]) {
		if (!encoded_bytes[1]) {
			printf("Base16-encoded string \"%s\" has invalid length\n",
					encoded);
			return -22;
		}
		memcpy(buf, encoded_bytes, 2);
		buf[2] = '\0';
		*(raw_bytes++) = strtoul(buf, &endp, 16);
		if (*endp != '\0') {
			printf("Base16-encoded string \"%s\" has invalid byte \"%s\"\n",
					encoded, buf);
			return -22;
		}
		encoded_bytes += 2;
	}
	len = (raw_bytes - raw);
	return (len);
}

int main(void) {
    printf("Hello\r\n");
    
    // Pass 3 # 1
    //char buf[] = {130, 167, 128, 0, 82, 62, 171, 219, 249, 0, 115, 255, 228, 251, 240, 0, 0, 255, 238, 0, 75, 255, 208, 255, 140, 255, 241, 0, 114, 0, 29, 0, 16, 6, 172, 255, 243, 255, 221, 255, 154, 217, 98, 19, 77, 3, 82, 96, 155, 14, 15, 14, 4, 30, 1, 32, 125, 0, 131, 0, 4, 0, 173, 0, 42, 0, 9, 0, 0, 0, 229, 0, 67, 5, 179, 1, 64, 0, 184, 255, 59, 255, 252, 255, 253, 255, 252, 255, 252, 255, 251, 29, 0, 75, 64, 0, 0, 64, 5, 0, 0, 0, 0, 2, 0, 4, 128, 0, 64, 0, 0, 4, 0, 2, 0, 2, 68, 5, 0, 25, 112, 0, 171, 125, 0, 128, 12, 0, 0, 0, 1, 128, 6, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 169, 134, 16, 108, 0, 1, 247, 0, 16, 5, 255, 0, 16, 0, 1, 66, 236, 23, 12, 193, 248, 151, 72, 226, 107, 89, 216, 98, 211, 248, 115, 65, 34, 219, 121, 67, 187, 15, 82, 195, 114, 147, 162, 66, 131, 4, 202, 2, 66, 170, 128, 0, 66, 170, 0, 32, 64, 170, 0, 0, 50, 34, 80, 57, 107, 14, 104, 32, 48, 2};
    
    // Pass 3 # 2
    //char buf[] = {130, 167, 128, 0, 82, 62, 172, 207, 249, 24, 115, 239, 248, 247, 248, 255, 255, 0, 23, 0, 94, 255, 226, 255, 163, 255, 255, 1, 36, 0, 67, 0, 18, 6, 92, 255, 249, 255, 255, 207, 158, 217, 40, 3, 74, 67, 82, 0, 27, 9, 181, 11, 168, 11, 159, 32, 106, 0, 135, 0, 6, 0, 83, 128, 40, 0, 9, 0, 0, 2, 79, 0, 56, 0, 100, 0, 224, 0, 151, 0, 0, 255, 254, 255, 255, 255, 254, 255, 252, 255, 251, 29, 4, 10, 0, 0, 0, 96, 5, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 64, 3, 0, 2, 0, 2, 2, 129, 0, 37, 0, 0, 173, 160, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 171, 195, 0, 12, 0, 1, 248, 0, 16, 4, 255, 0, 1, 0, 1, 66, 164, 135, 184, 194, 113, 242, 192, 70, 46, 49, 12, 66, 180, 245, 143, 65, 35, 242, 161, 70, 128, 195, 35, 195, 141, 125, 226, 67, 109, 199, 224, 2, 192, 170, 0, 0, 66, 170, 12, 0, 194, 170, 0, 8, 34, 34, 75, 107, 8, 50, 68, 18, 0, 2};
    
    // Pass 5
    //char buf[] = {130, 167, 128, 0, 82, 62, 216, 73, 249, 0, 115, 255, 220, 203, 228, 255, 251, 255, 129, 255, 163, 255, 249, 255, 164, 255, 187, 2, 111, 0, 137, 0, 28, 11, 113, 255, 251, 255, 251, 255, 159, 218, 34, 3, 74, 3, 82, 0, 27, 19, 174, 19, 161, 1, 225, 32, 87, 0, 129, 0, 6, 0, 81, 0, 42, 0, 9, 0, 0, 0, 33, 1, 164, 0, 64, 0, 230, 0, 149, 255, 235, 255, 251, 223, 250, 255, 250, 255, 253, 255, 252, 29, 0, 15, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 3, 0, 2, 0, 2, 0, 2, 0, 44, 12, 1, 32, 179, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 2, 16, 1, 29, 130, 0, 12, 0, 1, 213, 0, 16, 5, 255, 0, 25, 0, 1, 66, 164, 135, 128, 194, 113, 242, 192, 66, 46, 49, 12, 70, 244, 181, 143, 65, 35, 242, 161, 66, 128, 195, 35, 195, 141, 125, 194, 67, 109, 198, 160, 2, 194, 170, 0, 0, 66, 170, 0, 0, 194, 170, 0, 0, 34, 34, 75, 107, 8, 50, 4, 18, 0, 2};
    
    // Pass 6
    //char buf[] = {130, 167, 128, 3, 82, 60, 238, 209, 249, 0, 127, 253, 232, 255, 240, 254, 182, 247, 159, 255, 149, 255, 152, 255, 239, 255, 191, 14, 112, 0, 11, 0, 29, 13, 182, 255, 238, 255, 253, 255, 209, 207, 24, 43, 68, 1, 210, 96, 9, 28, 212, 156, 243, 59, 139, 32, 97, 32, 133, 1, 214, 0, 99, 4, 40, 0, 9, 2, 4, 144, 66, 1, 79, 0, 0, 1, 88, 0, 151, 71, 247, 63, 252, 255, 252, 253, 252, 255, 63, 255, 250, 61, 40, 11, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 64, 0, 0, 0, 0, 0, 3, 2, 2, 0, 2, 12, 2, 0, 33, 134, 1, 85, 78, 56, 0, 192, 0, 3, 0, 0, 0, 16, 0, 0, 0, 3, 1, 7, 32, 6, 0, 0, 0, 0, 1, 89, 47, 0, 12, 4, 1, 222, 0, 16, 5, 255, 0, 0, 224, 1, 66, 164, 143, 130, 82, 113, 242, 192, 68, 46, 49, 12, 68, 180, 189, 143, 81, 35, 242, 158, 95, 128, 131, 35, 195, 253, 77, 192, 91, 101, 198, 32, 2, 130, 171, 0, 0, 66, 202, 0, 4, 194, 170, 0, 0, 34, 42, 75, 99, 12, 48, 4, 18, 0, 2};
    
    // Pass 8
    //char buf[] = {130, 167, 128, 0, 82, 63, 202, 59, 249, 0, 115, 255, 236, 255, 244, 255, 255, 255, 243, 0, 9, 191, 250, 0, 9, 255, 250, 0, 0, 0, 4, 0, 6, 87, 241, 255, 253, 255, 253, 0, 0, 0, 0, 3, 74, 3, 80, 0, 156, 10, 187, 10, 175, 10, 165, 224, 106, 0, 149, 0, 6, 0, 98, 0, 40, 0, 9, 0, 1, 1, 250, 0, 223, 0, 153, 0, 226, 0, 151, 255, 255, 255, 254, 255, 254, 127, 253, 255, 253, 255, 253, 29, 32, 139, 110, 0, 0, 64, 13, 128, 0, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0, 3, 8, 24, 0, 24, 0, 23, 4, 192, 0, 4, 208, 106, 0, 66, 73, 83, 82, 63, 194, 105, 66, 61, 64, 92, 66, 33, 91, 230, 0, 0, 129, 1, 25, 4, 14, 251, 0, 12, 0, 1, 248, 0, 16, 5, 252, 0, 7, 0, 1, 66, 164, 143, 136, 194, 113, 242, 192, 66, 46, 49, 12, 66, 180, 181, 143, 65, 35, 242, 161, 66, 128, 195, 35, 195, 141, 125, 194, 67, 109, 198, 224, 2, 192, 170, 0, 0, 66, 170, 66, 0, 194, 170, 0, 0, 34, 34, 75, 107, 8, 50, 4, 146, 0, 2};
    
    // Pass 8 # 2
    //char buf[] = {134, 191, 137, 1, 90, 51, 224, 79, 249, 0, 115, 255, 169, 103, 244, 191, 255, 251, 251, 0, 1, 223, 249, 0, 20, 241, 33, 0, 26, 64, 42, 0, 6, 29, 245, 119, 219, 156, 253, 255, 155, 255, 48, 3, 74, 131, 80, 0, 157, 11, 32, 27, 39, 42, 89, 0, 107, 1, 131, 17, 6, 6, 88, 16, 40, 0, 71, 32, 0, 1, 37, 1, 199, 0, 97, 0, 146, 0, 151, 255, 231, 221, 251, 207, 255, 127, 253, 255, 251, 159, 61, 29, 0, 138, 115, 0, 24, 86, 5, 0, 0, 12, 2, 80, 0, 0, 16, 0, 0, 0, 8, 12, 0, 57, 0, 90, 2, 137, 0, 110, 0, 228, 210, 242, 24, 34, 66, 176, 82, 63, 200, 5, 64, 85, 101, 213, 70, 70, 241, 34, 0, 0, 21, 39, 232, 0, 50, 54, 0, 12, 0, 1, 248, 0, 16, 133, 252, 0, 145, 0, 9, 66, 168, 65, 136, 194, 112, 242, 224, 66, 78, 49, 12, 6, 244, 45, 143, 195, 3, 242, 161, 85, 128, 195, 35, 211, 141, 109, 98, 67, 105, 198, 112, 3, 194, 170, 0, 16, 65, 170, 4, 0, 135, 170, 24, 0, 34, 34, 73, 107, 12, 130, 20, 146, 0, 130};
    
    // Pass 12 #1
    //char buf[] = {0x82, 0xEC, 0xD8, 0x00, 0x52, 0x41, 0x1D, 0x7B, 0x71, 0x32, 0x73, 0xFF, 0xD0, 0xFE, 0xD4, 0xFF, 0xFF, 0xFF, 0xC7, 0xFF, 0xBD, 0xBF, 0x35, 0xFF, 0xCF, 0xFF, 0xDE, 0x01, 0x23, 0x00, 0x57, 0x00, 0x18, 0x12, 0xAE, 0xFF, 0xF7, 0xFF, 0xF7, 0xF7, 0x9A, 0x59, 0x62, 0x03, 0x48, 0x83, 0x4E, 0x00, 0xDB, 0x0A, 0xB2, 0x1C, 0xA6, 0x0A, 0x49, 0xE0, 0x68, 0x40, 0x04, 0x02, 0x06, 0x00, 0x52, 0x04, 0x2F, 0x20, 0x05, 0x02, 0x00, 0x02, 0xDC, 0x00, 0x41, 0x01, 0x0B, 0x00, 0x8C, 0x25, 0xDA, 0xFF, 0xF8, 0x2F, 0xF9, 0xD7, 0xF4, 0xFF, 0xF8, 0xFF, 0xFB, 0xFC, 0x7A, 0x1C, 0x48, 0x8B, 0x01, 0x20, 0x04, 0x00, 0x05, 0x42, 0x20, 0xC4, 0x20, 0x00, 0x00, 0x01, 0x02, 0x00, 0x80, 0x40, 0x08, 0x0B, 0x60, 0x16, 0x80, 0x26, 0x48, 0x2A, 0x07, 0xFF, 0x08, 0x86, 0x94, 0x8C, 0x00, 0x0C, 0xA5, 0x2B, 0x52, 0x3F, 0xF0, 0xEA, 0x42, 0x53, 0xB2, 0x4C, 0x43, 0xB1, 0x05, 0x38, 0x00, 0x00, 0x4D, 0x08, 0x00, 0x05, 0x15, 0x1E, 0x00, 0x0C, 0x00, 0x01, 0xF2, 0x80, 0x10, 0x07, 0xFC, 0x00, 0x81, 0x00, 0x01, 0x42, 0xA4, 0x87, 0x80, 0xC2, 0x71, 0xF2, 0xC4, 0x4A, 0x2E, 0x31, 0x0C, 0x42, 0xF4, 0x35, 0x0F, 0x41, 0x23, 0xFA, 0xA1, 0xF4, 0xB1, 0x3A, 0xB5, 0xF1, 0xB1, 0x96, 0x92, 0x42, 0x05, 0x2F, 0x1C, 0x02, 0xC2, 0x9A, 0x00, 0x20, 0x42, 0xAA, 0x00, 0x03, 0xC2, 0xB2, 0x00, 0x00, 0x24, 0x62, 0x4B, 0x7B, 0x08, 0x33, 0x94, 0x92, 0x00, 0x02};
    
    // Pass 12 #2
    //char buf[] = {0x83, 0xD0, 0xD8, 0x00, 0x52, 0x42, 0x38, 0xF9, 0x71, 0x22, 0x73, 0x80, 0x4C, 0x00, 0x58, 0xFF, 0xED, 0x00, 0x22, 0x02, 0x47, 0xF0, 0x7C, 0x21, 0x11, 0x8C, 0x1C, 0x01, 0x21, 0x00, 0x5B, 0x00, 0x0D, 0x18, 0x91, 0x00, 0x15, 0x00, 0x15, 0xFF, 0x9A, 0xB9, 0x62, 0x25, 0x4B, 0x03, 0x4A, 0x01, 0x96, 0x06, 0x55, 0x06, 0x4F, 0x06, 0xC1, 0xA0, 0xE8, 0x00, 0x0C, 0x06, 0x24, 0x00, 0x69, 0x00, 0x21, 0x20, 0x07, 0x00, 0x00, 0x01, 0x03, 0x01, 0xA1, 0x48, 0x82, 0xC0, 0x7A, 0x00, 0x42, 0xC0, 0x13, 0x00, 0x13, 0x00, 0x36, 0x08, 0x12, 0x06, 0xEC, 0x00, 0x0B, 0x1D, 0x0C, 0x89, 0x19, 0xA0, 0x00, 0x00, 0x0B, 0xC1, 0x28, 0x04, 0x18, 0x00, 0x08, 0x03, 0x03, 0x00, 0xC0, 0x40, 0x01, 0x03, 0x22, 0x15, 0x00, 0x24, 0x59, 0xAA, 0x07, 0xBD, 0x00, 0x07, 0xB4, 0x94, 0x08, 0x0C, 0xE5, 0x23, 0x56, 0x3F, 0xF0, 0xEA, 0x42, 0x55, 0x14, 0x4C, 0x43, 0xB1, 0x07, 0x38, 0x00, 0x02, 0x0D, 0x48, 0x11, 0x85, 0x3B, 0x0E, 0x02, 0x0C, 0x06, 0x01, 0x10, 0x00, 0x11, 0x03, 0xB4, 0x20, 0x01, 0x00, 0x01, 0x42, 0xA4, 0x87, 0x88, 0x42, 0x71, 0xF2, 0xC0, 0x42, 0x0E, 0x31, 0x0C, 0x42, 0xF4, 0xB5, 0x8F, 0xA1, 0x23, 0xF2, 0xA1, 0xDA, 0x32, 0x3A, 0x85, 0x43, 0xF1, 0x97, 0x92, 0x42, 0x08, 0x8F, 0x1C, 0x02, 0xC3, 0x2A, 0x00, 0x00, 0x42, 0xAA, 0x00, 0x00, 0xC6, 0xAA, 0x02, 0x00, 0x32, 0x20, 0x4A, 0x68, 0x08, 0x32, 0x04, 0x12, 0x00, 0x02};
    
    // Pass 13 #1
    //char buf[] = {130, 183, 128, 0, 82, 65, 120, 157, 249, 0, 115, 254, 92, 255, 200, 243, 223, 255, 136, 255, 119, 255, 129, 223, 135, 255, 167, 17, 214, 0, 94, 12, 28, 4, 227, 255, 246, 255, 251, 191, 153, 231, 8, 3, 50, 3, 78, 0, 29, 1, 235, 1, 166, 33, 197, 159, 133, 0, 144, 128, 225, 0, 105, 16, 38, 0, 53, 0, 1, 0, 0, 0, 0, 128, 64, 0, 0, 0, 245, 255, 120, 253, 234, 255, 249, 255, 250, 255, 252, 255, 251, 31, 0, 11, 2, 0, 24, 0, 5, 0, 0, 0, 0, 0, 0, 128, 0, 0, 0, 0, 0, 3, 8, 25, 0, 3, 32, 2, 3, 78, 8, 0, 22, 98, 16, 67, 167, 164, 82, 64, 104, 125, 67, 6, 240, 48, 65, 131, 129, 212, 0, 0, 136, 184, 0, 0, 153, 160, 0, 13, 0, 0, 246, 0, 17, 3, 124, 0, 1, 0, 1, 78, 220, 91, 64, 191, 229, 111, 1, 241, 131, 107, 152, 73, 5, 218, 140, 64, 189, 117, 163, 194, 96, 86, 37, 195, 233, 152, 154, 66, 153, 66, 46, 2, 67, 170, 0, 3, 82, 170, 0, 128, 82, 170, 0, 48, 34, 34, 132, 2, 4, 4, 5, 0, 0, 64};
    
    // Pass 13 #2
/*    char buf[] = {0x82, 0xC6, 0x1C, 0x00, 0x52, 0x70, 0x47, 0xB3, 0xF0, 0x11, 0x73, 0xFF, 0xE8, 0x3C, 0xF0, 0xFF, 0x8F, 0xFB, 0x90, 0xFF, 0xFF, 0xFF, 0x20, 0xF3, 0x8E, 0xFF, 0xAD, 0x09, 0x28, 0x00, 0x0A, 0x01, 0x0E, 0x44, 0x5E, 0x1F, 0xFB, 0x1F, 0xFD, 0x8B, 0x94, 0xDC, 0xC2, 0x23, 0x34, 0x81, 0x50, 0x00, 0x9D, 0x01, 0x0B, 0x61, 0xB8, 0x29, 0x42, 0x1D, 0xE3, 0x00, 0x0E, 0x30, 0x81, 0x06, 0xD8, 0x00, 0x28, 0x20, 0x15, 0x0E, 0x07, 0x0A, 0x30, 0x0A, 0x40, 0x03, 0xB9, 0x00, 0x01, 0x00, 0x86, 0xFF, 0xC9, 0xFF, 0x14, 0xFF, 0xCB, 0xFB, 0xFB, 0x4F, 0xF8, 0xFF, 0xFD, 0x9F, 0x03, 0x0B, 0xEF, 0x80, 0x04, 0x00, 0x05, 0x00, 0x00, 0x10, 0x00, 0x04, 0x01, 0x00, 0x60, 0x02, 0x00, 0x20, 0x00, 0x03, 0x00, 0x19, 0x61, 0x0B, 0xE0, 0x0A, 0x42, 0x6B, 0x00, 0x00, 0xD3, 0x23, 0x00, 0x2E, 0x92, 0x1C, 0x52, 0x58, 0xF7, 0xF6, 0x42, 0x6C, 0x0E, 0x79, 0x59, 0x12, 0x88, 0xED, 0xC0, 0x03, 0x4C, 0xDF, 0xD1, 0x47, 0x7F, 0x08, 0x00, 0x08, 0x8A, 0x00, 0x39, 0x08, 0x19, 0x01, 0x74, 0x21, 0x45, 0xA2, 0x1D, 0x46, 0x9F, 0x17, 0x20, 0xC0, 0x44, 0x5A, 0x22, 0x71, 0x9A, 0x03, 0x74, 0x49, 0x67, 0x27, 0xDF, 0x02, 0x06, 0x8B, 0xC4, 0xC7, 0xC7, 0x97, 0x7B, 0xC3, 0xEB, 0xFD, 0xE5, 0xCB, 0x47, 0xC7, 0xA4, 0x02, 0x4B, 0x28, 0x83, 0x60, 0x58, 0xEF, 0x48, 0xB6, 0xFF, 0x8A, 0x33, 0x19, 0x41, 0xC4, 0xC5, 0xB2, 0x4A, 0x48, 0x66, 0x18, 0x28, 0x43};
*/

char buf [] ={0x82, 0xA7, 0x80, 0x00, 0x52, 0x41, 0xA6, 0x19, 0xF9, 0x00, 0x73, 0xFF, 0xEC, 0xFF, 0xF4, 0xFF, 0xFF, 0xFF, 0x85, 0xFF, 0x8B, 0xBF, 0xA6, 0xFF, 0xC5, 0xFF, 0xB1, 0x02, 0x9F, 0x00, 0xB5, 0x00, 0x23, 0x5B, 0x24, 0x00, 0x00, 0xFE, 0xFF, 0xFF, 0x91, 0x02, 0x5C, 0x23, 0x30, 0x03, 0x4E, 0x00, 0x1E, 0x1C, 0xED, 0x01, 0xDF, 0x0E, 0x70, 0x1F, 0x7B, 0x00, 0x9A, 0x00, 0x82, 0x00, 0xB3, 0x10, 0x2A, 0x00, 0x12, 0x00, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00, 0x01, 0x1A, 0xFF, 0xFC, 0xFF, 0xFD, 0xDF, 0xF4, 0xFF, 0xFD, 0x00, 0x00, 0xFF, 0xFE, 0x1F, 0x00, 0x0B, 0x00, 0x00, 0x00, 0x40, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x07, 0x00, 0x08, 0x00, 0x15, 0x0E, 0x2C, 0x00, 0x0E, 0xBD, 0x2C, 0x00, 0x42, 0x48, 0xF5, 0x52, 0x41, 0xA6, 0x4B, 0x42, 0x5E, 0x3F, 0x7E, 0x41, 0xFE, 0xE0, 0x6B, 0x00, 0x00, 0x8C, 0x87, 0x00, 0x08, 0x62, 0x87, 0x01, 0x0D, 0x00, 0x01, 0xD9, 0x00, 0x11, 0x01, 0xFC, 0x00, 0x01, 0x00, 0x01, 0xBF, 0x10, 0xF2, 0x00, 0xC0, 0x12, 0x0C, 0xC0, 0xBD, 0xAB, 0x95, 0x60, 0x40, 0xF5, 0x3B, 0x40, 0x40, 0x04, 0xE4, 0x4B, 0xC3, 0xCD, 0xA9, 0x5D, 0x43, 0x43, 0x80, 0x75, 0xC2, 0x50, 0x14, 0x5A, 0x03, 0x42, 0xAA, 0x00, 0x00, 0x42, 0x9A, 0x40, 0x08, 0x40, 0xAA, 0x00, 0x00, 0x22, 0x22, 0x0A, 0x56, 0x07, 0x05, 0x27, 0x00, 0x00, 0x00};

    printf("Size %lu\r\n", sizeof(buf));
    printf("Beacon size %lu\r\n", sizeof(cdh_beacon));
    cdh_beacon * b = (void *) &buf[4];
    cdh_beacon_ntoh(b);
    cdh_beacon_print(b);
   
}
