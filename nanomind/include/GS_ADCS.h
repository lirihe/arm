/** \file
 * \author Lars Alminde
 * \author Jesper A. Larsen
 * \version GomSpace 2011
 * \brief ADCS framework definitions
 */

#ifndef GS_ACDS_H_
#define GS_ACDS_H_

#ifdef _MSC_VER
#include "stdint.h"
#else
#include <inttypes.h>
#endif

//#include "GS_ADCS_Param.h"
#include <adcs/adcs_types.h>
#include <stdio.h>
#include <math.h>

//Configuration options
//---------------------------------------------------------------------------

#if defined(__arm__)
#define A712
#elif defined(__i386__)
#define X86
#else
#define MATLAB
#endif

extern void GS_ACDS_ErrorReport(char *txt, ...);

#ifndef M_PI
#define M_PI 3.141592653589793
#endif

#ifdef __WATCOMC__
double roundf(double x);
#endif




typedef struct {
  FILE *logFile;		/**< Pointer to log file */
  bool_t logMode;               /**< Mode of logger 0=stopped, 1=running */
  uint32_t  logSamples;         /**< Count of samples in current log */
  int runoffset;
  int logTimeRemaining;         /**< Time remaining for log (runcounter) */
  unsigned int logTime;		/**< Period of logging [s] */
  unsigned int logDivider;	/**< How often should the logging run as a multiplum of ADCS iterations */
  uint8_t logType[NR_OF_ADCS_LOG_TYPES];           /**< [0]: 1=log bdot,0=not log. [1]: 1=log ephem 0=not log. [2]: 1=log kf, 2=log kfext, 0=not log. [3]: 1=log lqr, 0=not log */
} GS_ADCS_Logdata_t;



// Actuator data
//---------------------------------------------------------------------------
/**
 *
 */
typedef struct {
	adcs_float_t  torquerDutyCycle[3];
} GS_ADCS_ActuatorData_t;




// Raw sensor data
//---------------------------------------------------------------------------
/**
 * Structure used to contain the raw sensor data along with the current status
 */
typedef struct {
  float  magnetometer[3];					/**< The magnetometer measurementes [mGauss] */
  float  magnetometer_raw[3];				/**< The RAW magnetometer measurements [mGauss] */
  bool_t magnetometerValid;		        	/**< True if the magnetometer was measured during this iteration */
  signed int sunsensor[6];		        	/**< The sunsensor measurements [bits 0-1023] */
  signed int sunsensor_raw[6];		        /**< The raw sunsensor measurements [raw bits 0-1023] */
  bool_t sunsensorValid;					/**< True if the sunsensors were measured during this iteration */
  signed int sunsensorTemperatures[6];   	/**< The raw sunsensor temperature measurements [raw bits 0-1023]*/
  bool_t sunsensorTemperaturesValid;    	/**< True if the sunsensor temperature sensors were measured during this iteration */
  float  gyro[3];							/**< The gyro measurements [deg/s]*/
  float  gyro_raw[3];						/**< The raw gyro measurements [deg/s]*/
  bool_t gyroValid;							/**< True if the gyros were measured during this iteration */
} GS_ADCS_MeasurementsData_t;

/**
 * Structure used to contain work data for the ADCS
 */
typedef struct {
  int8_t RunMode;					/**< 0=ADCS stopped, 1=ADCS running */
  int RunCounter;	      			/**< Count up on each ADCS iteration */
  adcs_float_t magS[3];	            /**< Mag field in S-frame  */
  uint8_t DoRestart;                 /**< Set true for a restart of ADCS system */
  unsigned int looptime;            /**< Average ADCS loop time in ms (over 50 runs)*/
  unsigned int maxlooptime;			/**< Maximum ADCS loop time in ms (over the last 50 runs) */
} GS_ADCS_WorkData_t;





// Output data-structure
//---------------------------------------------------------------------------
/**
 * The ADCS output structure.
 */
typedef struct {
  adcs_float_t bdotRateEstimate[2];
  adcs_float_t bdotRateVectorEstimate[3];
  adcs_float_t quaternionEstimate[4];
  adcs_float_t angularVelocityEstimate[3];
  adcs_float_t magnetometerBiasEstimate[3];
  adcs_float_t magnetometerScaleEstimate[3];
  adcs_float_t sunsensorMinEstimate[6];
  adcs_float_t sunsensorMaxEstimate[6];
  adcs_float_t quasiTorqueEstimate[3];
  adcs_float_t torquerDutyCycle[3];
  adcs_float_t ctrlTorques[3];
  adcs_float_t julianDate;

  //Outputs for simulation
  adcs_float_t EstimatedState[16];

  // Ctrl output
  adcs_float_t CtrlError[15]; // QuaternionRef[4];  QuaternionError[4]; RateError[3]; iquaterr[4]
} GS_ADCS_OutputData_t;




// Function prototypes
//---------------------------------------------------------------------------

/**
 * Initialize the ADCS configurations struct with default values.
 * Also sets up the the orbit propagator and checks that the time is valid.
 * @return n-zero on error.\n
 *                   -1 - mean elements, ecc >= 1.0 or ecc < -0.001 or a < 0.95 er \n
 *                   -2 - mean motion less than 0.0 \n
 *                   -3 - pert elements, ecc < 0.0  or  ecc > 1.0 \n
 *                   -4 - semi-latus rectum < 0.0 \n
 *                   -5 - epoch elements are sub-orbital \n
 *                   -6 - satellite has decayed \n
 *					 -7 - Julian date is before 01-01-2011
 *
 */
int GS_ADCS_initialise(void);


void GS_ADCS_SetWorkParam(GS_ADCS_Config_t *config);


double adcs_get_jdate(void);



#endif /* GS_ACDS_H_ */
