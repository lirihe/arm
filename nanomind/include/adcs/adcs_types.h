/*
 * adcs_client_types.h
 *
 *  Created on: May 25, 2011
 *      Author: jal
 */

#ifndef ADCS_CLIENT_TYPES_H_
#define ADCS_CLIENT_TYPES_H_

#include <inttypes.h>

typedef float adcs_float_t;

#ifndef true
# define false 0
# define true 1
#endif
typedef unsigned short bool_t;


#define MAX_ADCS_CONFIG_SIZE 210  // Maximum number of bytes allowed ina config struct
#define MAX_ADCS_TLE_LINE 70      // Length of TLE line

#define NR_OF_ADCS_LOG_TYPES 5

/* Type def for quaternion type defaulting to unit quaterion*/
typedef struct {
    adcs_float_t q1;
    adcs_float_t q2;
    adcs_float_t q3;
    adcs_float_t q4;
} quaternion_t;

/**
 * Control mode enumeration
 * @note: These types are used in libadcs too
 */
typedef enum __attribute__ ((packed)) ControlMode_t{
	ACS_IDLE, 	/**< ACS idle mode */
	ACS_CONST, 	/**< ACS constant actuation mode */
	ACS_BDOT, 	/**< ACS Bdot mode, i.e. detumbling mode */
	ACS_NADIR, 	/**< ACS Nadir pointing mode */
	ACS_INERTIAL, 	/**< ACS Inertial pointing mode */
} ControlMode_t;

/**
 * Attitude determination mode enumeration
 * @note: These types are used in libadcs too
 */
typedef enum __attribute__ ((packed)) DeterminationMode_t{
	ADS_IDLE, 			/**< ADS idle mode */
	ADS_MAG, 			/**< ADS magnetometer only mode */
	ADS_ALL_SENSORS, 	/**< ADS sample all sensors mode */
	ADS_KALMANFILTER	/**< ADS kalman filter estimation mode */
} DeterminationMode_t;

// ACDS Mode Definition
//---------------------------------------------------------------------------

/**
 * Current and desired determination and control modes
 */
typedef struct {
  ControlMode_t controlMode;						/**< Current control mode */
  ControlMode_t desiredControlMode;				/**< Desired control mode if possible */
  DeterminationMode_t determinationMode;			/**< Current determination mode */
  DeterminationMode_t desiredDeterminationMode;	/**< Desired determination mode if possible */
} GS_ADCS_Mode_t;


/**
 * Structure used to contain status for the ADCS
 */
typedef struct { 
  /* IGRF status 0=OK, -1=Error */
  signed short igrf;
  /* SGP4 status:     0 - OK
   *                 -1 - mean elements, ecc >= 1.0 or ecc < -0.001 or a < 0.95 er
   *                 -2 - mean motion less than 0.0
   *                 -3 - pert elements, ecc < 0.0  or  ecc > 1.0
   *                 -4 - semi-latus rectum < 0.0
   *                 -5 - epoch elements are sub-orbital
   *                 -6 - satellite has decayed */
  signed short sgp4;
  /* Time Error: 0=OK, -1=time is too early */
  signed short time;
} GS_ADCS_Ephem_Status_t;

typedef struct {  
  /* Magnetometer status: 0=OK, -1=mag read error, -2=calib index error, -3=value error*/
  signed short mag;
  /* Sunsensor status: 0=OK,  */
  signed short sun;
  /* Gyro status: 0=OK,  */
  signed short gyro;
} GS_ADCS_Sensor_Status_t;

typedef struct {
  /**< Current sensor status  */
  GS_ADCS_Sensor_Status_t sensorStatus;
  /**< Current bdot status.  */
  signed short bdotStatus;
  /**< Current UKF status. 0=OK, -1=nan in estimated state, -2=nan in measurement, -3=wrong array sizes  */		
  signed short ukfStatus;
  /**< Current ephemeris status.  */		
  GS_ADCS_Ephem_Status_t ephemStatus;
  /**< Current parameter file status: 2=AutoWrittenFile, 1=UploadedFile, 0=Problem in uploaded file, using partly defaults */
  signed short paramStatus; 	
  /**< Current run status on ADCS. 0=stopped, 1=running, -1=dead*/
  signed short runStatus;	
} GS_ADCS_Status_t;



// Ephemeris
//---------------------------------------------------------------------------
typedef struct {
  double julianDate;
  adcs_float_t scRECI[3];  // Space craft pos in ECI
  adcs_float_t scVECI[3];  // Space craft vel in ECI
  adcs_float_t sunECI[3];  // Sun vector 
  adcs_float_t magECI[3];  // Magnetic field [nT]
  adcs_float_t dmagECI[3]; // Deriviative of Magnetic field [nT]
  quaternion_t rotationIE; // Rotation from ECI to ECEF frame
  int timeSinceEclipse;    // Number of samples since satellite came out of eclipse - if 0 then we're in eclips. Max value: 10.000
  quaternion_t rotationIO; // Rotation from ECI to orbit frame
  adcs_float_t rateIO[3];  // Rotation rate from ECI to orbit frame
} GS_ADCS_Ephem_t;



// CTRL
//---------------------------------------------------------------------------
// Type def for lqr controller
typedef struct {
  quaternion_t ref_q;       // Reference quaternion
  quaternion_t err_q;       // Error quaternion
  quaternion_t ierr_q;      // Integral error quaternion
  adcs_float_t err_rate[3]; // Error rate [rad/s}
  adcs_float_t M[3];        // Control signal, magnetic moment [Am^2]
} GS_ADCS_LQR_t;

typedef struct {
  adcs_float_t boreError;   // Pointing bore error [rad]
  unsigned int counter;
  bool_t upsidedown;        // 1=upside down, 0=right way up  
  bool_t flip;              // 1=flipping controller, 0=lqr
} GS_ADCS_flip_t;

// ACDS Configuration
//---------------------------------------------------------------------------


typedef  struct {
  adcs_float_t inertia[3];				/**< Satellite inertia along the three principal axes */
  quaternion_t qSSC;					/**< Rotation from sensor frame to SC frame > **/
  quaternion_t qSCC;					/**< Rotation from SC frame to Control frame > **/
} GS_ADCS_Config_sc_t;


typedef  struct {
  ControlMode_t desiredControlModeInit;			/**< Desired control mode if possible */
  DeterminationMode_t desiredDeterminationModeInit;	/**< Desired determination mode if possible */  
  uint16_t adcsSampleTime; 		       /**< The ADCS sample time in ms  */
  uint16_t ukfDivider;  		       /**< How often should the UKF be run as a multiplum of ADCS iterations*/
  uint16_t ephemerisDivider;		       /**< How often should the ephemeris calculations be run as a multiplum of ADCS iterations*/
  uint16_t bdotDivider;		               /**< How often should the bdot be run as a multiplum of ADCS iterations */
  uint16_t ctrlDivider;		               /**< How often should the tracking controller be run as a multiplum of ADCS iterations */
} GS_ADCS_Config_sv_t;


typedef struct {
  float bdotMaxBField;         	              /**< maximum expected field strength in nT */
  adcs_float_t bdotTimeConstant;              /**< coefficient of averaging for BDOT filter - shall be between 0 and 1 */
  adcs_float_t bdotTimeConstant2;             /**< coefficient of averaging for rate norm filter */
  adcs_float_t bdotFilterTimeSeperation;      /**< Time seperation between fast and slow filters, must be larger than 1*/
  unsigned short int bdotBiasAxis;            /**< on which axis is the bias 0=x, 1=y, 2=z */
  unsigned short int bdotBiasStrength;        /**< 0-100 bias strength */
  signed short int bdotBiasDirection;         /** direction, -1 or 1 */
  adcs_float_t bdotThreshold[3]; 	      /**< actuation thresholds nT/s - for each axis */
  bool_t bdotBangBang;                        /**<  Should bdot be a BangBang controller (1=fast actuation) or use scaled-feedback 0=gentler actuation) */
} GS_ADCS_Config_bdot_t;


typedef struct{
  adcs_float_t sunsensorMaxInit[6];    	       /**< Current maximum value for the sunsensor in the sunsensor max tracking algorithm */
  adcs_float_t sunsensorAbsMax[6];	       /**< Maximum permissible value for the sunsensor in the sunsensor max tracking algorithm */
  adcs_float_t sunsensorDarkTh;	               /**< Threshold for dark (eclipse) detection */
  adcs_float_t modelNoiseQ;
  adcs_float_t modelNoiseW;
  adcs_float_t modelNoiseBMag;
  adcs_float_t modelNoiseBGyro;
  adcs_float_t modelNoiseT;
  adcs_float_t sensorNoiseSun;
  adcs_float_t sensorNoiseMag;
  adcs_float_t sensorNoiseGyro;
  adcs_float_t initCovarQ;
  adcs_float_t initCovarW;
  adcs_float_t initCovarBMag;
  adcs_float_t initCovarBGyro;
  adcs_float_t initCovarT;
  bool_t UseNoChangeDisable;    // Enables/Disables that no change in a sensor measurement will disable it in the meas update 
  bool_t EnableDistTorqueEst;   // Enables/Disables disturbance torque estimation in UKF
  bool_t EnableCtrlTorqueEst;   // Enables/Disables the use of control torque in UKF model
  bool_t EnableGyros;           // Enables/Disables the use of gyros in the UKF
  short int GyroChoice;         // Which gyros to use: 0=Standard panel mount, 1=NanoHub gyros
  short int teme2eci;				// 0=Do not do teme2eci conversion, 1=Do teme2eci conversion
  short int normmag;				// 0=Do not normalize mag, 1=normalize mag
  short int normmagbias;			// 0=Do not normalize mag bias, 1=normalize mag bias
} GS_ADCS_Config_kf_t;





typedef struct {
  adcs_float_t magOffset[3];	/**< Magnetometer offset vector [x y z] */
  adcs_float_t magScale[3];		/**< Magnetometer scale vector [x y z] */
  adcs_float_t magRotate[9];	/**< Magnetometer scale rotation matrix */
  adcs_float_t gyroScale[3];
  adcs_float_t gyroOffset[3];
  adcs_float_t sunScale[6];     /**< Sunsensor scale vector [+x +y +z -x -y -z] */
  adcs_float_t sunOffset[6];    /**< Sunsensor offset vector [+x +y +z -x -y -z] */
  int8_t magSign[3];		/**< Magnetometer sign vector [x y z] */
  int8_t gyroSign[3];   	/**< Magnetometer sign vector [x y z] */
  int8_t magAxis[3];    	/**< Magnetometer axis vector [x y z] */
  int8_t gyroAxis[3];   	/**< Gyro axis vector [x-spiport y-spiport z-spiport] */
  int8_t sunAxis[6];    	/**< Sunsensor axis vector (adc channel) [+x +y +z -x -y -z] */
  int8_t magGain;		/**< Global magnetometer gain (0 ... 7) */
  int8_t magEnable[3];    	/**< Magnetometer enable vector (0=disable, 1=enable) [+x +y +z] */
  int8_t gyroEnable[3];    	/**< Gyro enable vector (0=disable, 1=enable) [+x +y +z] */
  int8_t sunEnable[6];    	/**< Sunsensor enable vector (0=disable, 1=enable) [+x +y +z -x -y -z] */
} GS_ADCS_Config_sensor_t;

typedef struct {
  adcs_float_t TorquerAM[3];         	  	/**< Area of torquers */
  unsigned short int TorquerAxis[3];		/**< Torquer axis vector [x y z] */
  short int TorquerSign[3];     	  		/**< Sign of torquers (+-1)[x y z] */
  uint16_t TorquerMax;						/**< In percent */
} GS_ADCS_Config_act_t;





typedef struct {
  adcs_float_t detumblingToPointingThreshold;  /**< Threshold for going from detumbling mode to pointing mode */
  adcs_float_t pointingToDetumblingThreshold;  /**< Threshold for going from pointing mode to detumbling mode */
  adcs_float_t quatKnadir[3*3];	/**< Proportional gain on the attitude error */
  adcs_float_t velKnadir[3*3];	/**< Proportional gain on the angular velocity error */
  adcs_float_t quatIKnadir[3*3];/**< Integral gain on the attitude error */
  adcs_float_t integralErrorBound[3];	/**< Bounds on the integral errors. The first three are for the attitude integral error, and the last three are for the integral angular velocity error */
  adcs_float_t constCtrl[3];		/**< Constant dutycycle for the three magnetorquers */
  bool_t ControlThroughEclipse;         /**< 1=Do control through eclipse, 0=do not control through eclipse*/ 
  int    EclipseDelayToControl;         /**< ADCS iterations to wait before trusting attitude estimate and start actuating. */
} GS_ADCS_Config_ctrl1_t;


typedef struct {
  adcs_float_t quatKinertial[3*3];	/**< Proportional gain on the attitude error */
  adcs_float_t velKinertial[3*3];	/**< Proportional gain on the angular velocity error */
  adcs_float_t quatIKinertial[3*3];     /**< Integral gain on the attitude error */
  quaternion_t refInertial;          /**< Quaternion Reference for Inertial controller  */
  adcs_float_t flipGain[2];
  adcs_float_t flipTh[2];
  bool_t flipEnable;
} GS_ADCS_Config_ctrl2_t;


// Only for internal operations, is calculated on the basis of the other config structs
typedef struct {
  adcs_float_t invInertia[3];	        /**< The inverse of the satellites inertias - defined for ease of computation */
  adcs_float_t magRotateTranspose[9];	/**< Magnetometer scale rotation matrix (transposed) */
  adcs_float_t TorquerDutyScale;        /**< Scale factor for for torquer duty cycle, necessary because of pwm dead time */
  short int TorquerSaturation[3];     	/**< Saturation value of torquers */
} GS_ADCS_Config_work_t;


// Type def for TLE
typedef struct {
  char line1[MAX_ADCS_TLE_LINE];
	char line2[MAX_ADCS_TLE_LINE];
	bool_t tleValid;
} GS_ADCS_TLE_t;


/**
 * Configuration struct, which holds all basic configuration parameters for the GS_ADCS system
 */
typedef struct {

  GS_ADCS_Config_sc_t sc;
  GS_ADCS_Config_sv_t sv;
  GS_ADCS_Config_kf_t kf;
  GS_ADCS_Config_ctrl1_t ctrl1;
  GS_ADCS_Config_ctrl2_t ctrl2;
  GS_ADCS_Config_bdot_t bdot;
  GS_ADCS_Config_sensor_t sensor;
  GS_ADCS_Config_act_t act;
  GS_ADCS_Config_work_t work;
} GS_ADCS_Config_t;



/**
 * Define maximum sizes for arrays in client/server packets
 */
#define UKF_MAX_NUM_X 16				// Number of error states - real part of quaternion is not included!
#define UKF_MAX_NUM_ERR 15				// Number of error states - real part of quaternion is not included!
#define UKF_MAX_NUM_Z 9					// z = [v_sun v_mag omega_gyro]
#define UKF_MAX_NUM_SENS 12				// 6xsunsensor,3xmag,3xgyro
#define MAX_NUM_SP (UKF_MAX_NUM_ERR*2+1)




/*
 * General commands
 */

typedef enum __attribute__ ((packed)) {
  ADCS_MATRIX_K,
  ADCS_MATRIX_PEST,
  ADCS_MATRIX_Q,
  ADCS_MATRIX_R,
} adcs_matrix_type_t;


typedef struct {
	uint8_t rows;
	uint8_t cols;
	uint8_t parts;
} adcs_matrix_size_t;

typedef struct {
	adcs_matrix_type_t type;
	uint8_t part;
} adcs_matrix_getpart_t;

typedef struct {
	float A[50];
} adcs_matrix_part_t;


typedef enum __attribute__ ((packed)) {
  ADCS_CONFIG_SV,
  ADCS_CONFIG_SC,
  ADCS_CONFIG_BDOT,
  ADCS_CONFIG_KF,
  ADCS_CONFIG_CTRL1,
  ADCS_CONFIG_CTRL2,
  ADCS_CONFIG_SENSOR,
  ADCS_CONFIG_ACT,
} adcs_config_type_t;

typedef struct {
	uint8_t	mode; // 0=stop, 1=start, 2=restart
} adcs_startstop_t;

typedef struct {
	uint8_t  ADSstate;
	uint8_t  dADSstate;
	uint8_t  ACSstate;
	uint8_t  dACSstate;
} adcs_allstate_t;

typedef struct {
	uint8_t  state;
} adcs_state_t;

typedef struct {
  int32_t logtime;
  int32_t logdivider;
  uint8_t logtype[NR_OF_ADCS_LOG_TYPES];
  char file_name[26];
} adcs_log_req_t;

typedef struct {
	int32_t num_samples;
	uint32_t time_remain;
	uint8_t logtype[NR_OF_ADCS_LOG_TYPES];
} adcs_log_status_t;

typedef struct __attribute__ ((packed)) {
	double jDate;
	bool_t	validTime;
	uint32_t looptime;
	uint32_t maxlooptime;
} adcs_time_t;

 

/*
 * BDOT commands
 */

typedef struct {
	float tumblerate[3];
	float tumblenorm[2];
	float torquerduty[3];
	float mag[3];
	bool_t magvalid;
    bool_t detumbled;
} adcs_bdot_data_t;



/*
 * Sensor commands
 */

typedef struct {
	float mag[3];
} adcs_sensor_get_magnetometer_t;

typedef struct {
	uint16_t sunsensor[6];
} adcs_sensor_get_sunsensor_t;

typedef struct {
	float gyro[3];
} adcs_sensor_get_gyro_t;

typedef struct {
	uint16_t temperature[6];
} adcs_sensor_get_temperature_t;


/*
 * Actuator commands
 */

typedef struct {
	float pwm[3];
} adcs_act_get_torquers_t;

/*
 * UKF commands
 */

typedef struct {
	adcs_float_t ts;                             // Timestep for UKF
	bool_t kf_converged;                         //
	long int iterations;                         // Number of iterations by the UKF
	adcs_float_t sunsensor_max[6];               // Current maximum value for the sunsensor max tracking algorithm
	bool_t ineclipse;                            // UKF eclipse detection
} adcs_ukf_get_data_t;


typedef struct {
	uint8_t dimensions;
	float measurements[UKF_MAX_NUM_SENS];
	bool_t enabled[UKF_MAX_NUM_SENS];
} adcs_ukf_get_measurements_t;

typedef struct {
	uint8_t dimensions;
	float state[UKF_MAX_NUM_X];
} adcs_ukf_get_state_t;

typedef struct {
	uint8_t dimensions;
	float filtmeasurements[UKF_MAX_NUM_Z];
} adcs_ukf_get_filtmeasurements_t;


/*
 * Ephemeris commands
 */


typedef struct {
	adcs_float_t scpos[3];
} adcs_ephem_scpos_t;

typedef struct {
	adcs_float_t scvel[3];
} adcs_ephem_scvel_t;

typedef struct {
	adcs_float_t sunpos[3];
} adcs_ephem_sunpos_t;

typedef struct {
	adcs_float_t mag[3];
} adcs_ephem_mag_t;

typedef struct {
	quaternion_t q_i_o;
} adcs_ephem_q_i_o_t;

typedef struct {
	quaternion_t q_i_e;
} adcs_ephem_q_i_e_t;

typedef struct {
	uint16_t eclipsetime;
} adcs_ephem_eclipsetime_t;


/** Maximum path length for storage commands */
#define ADCS_MAX_PATH_LENGTH 50

/** List id type */
typedef uint16_t adcs_id_t;


/** Server commands */
typedef enum __attribute__ ((packed)) {
	/* General ADCS commands */
	ADCS_CMD_STARTSTOP,			       	/**< Do a start/stop/restart of ADCS system		*/
	ADCS_CMD_GET_ADCS_STATE,			/**< Get current ADCS states 				*/
	ADCS_CMD_SET_ADS_STATE,				/**< Set descired ADS state 				*/
	ADCS_CMD_SET_ACS_STATE,				/**< Set descired ACS state 				*/
	ADCS_CMD_GET_ADCS_STATUS,			/**< Get current ADCS status variables 			*/
	ADCS_CMD_GET_TIME ,   				/**< Get current time and valid flag		 	*/
	ADCS_CMD_LOG_REQ,					/**< Request a new logging	        	     	*/
	ADCS_CMD_LOG_ABORT,					/**< Abort the current logging request > **/
	ADCS_CMD_LOG_STATUS,				/**< Get status for current log >**/

	/* Matrix cmds */
	ADCS_CMD_GET_MATRIX_SIZE,
	ADCS_CMD_GET_MATRIX_PART,

	/* Parameter commands */
	ADCS_CMD_PARAM_SAVE,			       	/**< Write parameters to file    		       	*/	
	ADCS_CMD_PARAM_LOAD,			       	/**< Load parameters from file    			*/	
	ADCS_CMD_PARAM_SAVEDEFAULT,		        /**< Write parameters to default file    	  	*/	
	ADCS_CMD_PARAM_LOADDEFAULT,		       	/**< Load parameters from default file    		*/	
	ADCS_CMD_PARAM_GET_SC,			       	/**<    			*/
	ADCS_CMD_PARAM_SET_SC,			        /**<     			*/
	ADCS_CMD_PARAM_GET_SV,			       	/**<     			*/
	ADCS_CMD_PARAM_SET_SV,			        /**<   			*/
	ADCS_CMD_PARAM_GET_BDOT,			       	/**<    			*/
	ADCS_CMD_PARAM_SET_BDOT,			        /**<   			*/
	ADCS_CMD_PARAM_GET_KF,			       	/**< 			*/
	ADCS_CMD_PARAM_SET_KF,			        /**< 			*/
	ADCS_CMD_PARAM_GET_CTRL1,			       	/**< 			*/
	ADCS_CMD_PARAM_SET_CTRL1,			        /**< 		*/
	ADCS_CMD_PARAM_GET_CTRL2,			       	/**< 			*/
	ADCS_CMD_PARAM_SET_CTRL2,			        /**< 		*/
	ADCS_CMD_PARAM_GET_SENSOR,			       	/**< 			*/
	ADCS_CMD_PARAM_SET_SENSOR,			        /**< 			*/
	ADCS_CMD_PARAM_GET_ACT,			       	/**<		*/
	ADCS_CMD_PARAM_SET_ACT,			        /**<			*/
	ADCS_CMD_PARAM_GET_WORK,			       	/**< 			*/

        /* BDOT commands */
	ADCS_CMD_BDOT_GET_TUMB_RATE,			/**< Get estimated tumbling rate		       	*/
	ADCS_CMD_BDOT_FORCE_DETUMBLED,			/**< Force BDOT filters into detumbled state	       	*/

	/* Actuator commands */
	ADCS_CMD_ACT_GET,					/**< Get latest actuator values			*/

	/* Sensor commands */
	ADCS_CMD_SENSOR_INIT,			        /**< Perform an init on sensors 			*/
	ADCS_CMD_SENSOR_MAG_GET,				/**< Get latest magnetometer reading		*/
	ADCS_CMD_SENSOR_MAG_GET_RAW,			/**< Get latest magnetometer reading		*/
	ADCS_CMD_SENSOR_SUN_GET,				/**< Get latest sunsensor measurements		*/
	ADCS_CMD_SENSOR_SUN_GET_RAW,			/**< Get latest sunsensor measurements		*/
	ADCS_CMD_SENSOR_GYRO_GET,				/**< Get latest gyro measurements			*/
	ADCS_CMD_SENSOR_GYRO_GET_RAW,			/**< Get latest gyro measurements			*/
	ADCS_CMD_SENSOR_TEMP_GET,				/**< Get temperatures from the sidepanels	*/

	/* UKF commands */
	ADCS_CMD_UKF_GET_DATA,					/**< Get current UKF basic data			*/
	ADCS_CMD_UKF_GET_STATE,			        /**< Get current state vector	       	*/
	ADCS_CMD_UKF_GET_MEASUREMETS,			/**< Get current measurement vector    	*/
	ADCS_CMD_UKF_GET_FILTMEASUREMENTS,		/**< Get current prefiltered measurements	       	*/
	/* Ephemeris commands */
	ADCS_CMD_EPHEM_GET_TLE,			        /**< Get current TLE 					  */
	ADCS_CMD_EPHEM_SET_TLE,		  	        /**< Set TLE 						  */
	ADCS_CMD_EPHEM_SAVE_TLE,		        /**< Save current TLE to file     			  */
	ADCS_CMD_EPHEM_LOAD_TLE,	  	        /**< Load TLE from file					  */
	ADCS_CMD_EPHEM_GET_SCPOS,			/**< Get SC pos. in I [m]				  */
	ADCS_CMD_EPHEM_GET_SCVEL,			/**< Get SC vel. in I [m/s] 				  */
	ADCS_CMD_EPHEM_GET_SUNPOS,			/**< Get SUN pos. in I [m] 				  */
	ADCS_CMD_EPHEM_GET_MAG,				/**< Get B field in I [nT] 				  */
	ADCS_CMD_EPHEM_GET_QIO,				/**< Get rotation from I to O				  */
	ADCS_CMD_EPHEM_GET_QIE,				/**< Get rotation from I to E 				  */
	ADCS_CMD_EPHEM_GET_ECLIPSETIME,			/**< Get number of ephemeris iterations since eclipse     */

	/* Ctrl commands */
	ADCS_CMD_CTRL_RESET_INT,			/**< Reset integrators     */
	ADCS_CMD_CTRL_GET_DATA, 			/**< Get control data      */
	ADCS_CMD_CTRL_GET_FLIPDATA, 			/**< Get flip-controller data      */
	  

} adcs_server_cmd_t;




/** Server reply types */
typedef enum __attribute__ ((packed)) {
	ADCS_REPLY_OK,						/**< No error 						*/
	ADCS_REPLY_NOENT, 					/**< No such entry 					*/
	ADCS_REPLY_INVAL, 					/**< Invalid argument 				*/
} adcs_server_reply_t;


/*
 * adcs server packet
 */
typedef struct  __attribute__ ((packed)) {
  adcs_server_cmd_t cmd;
  adcs_server_reply_t reply;
  union {
    /* General commands */
    union {
      adcs_startstop_t startstop;
      adcs_allstate_t get_adcs;
      adcs_state_t set_ads;
      adcs_state_t set_acs;
      GS_ADCS_Status_t status;
      adcs_time_t time;
      adcs_log_req_t log_req;
      adcs_log_status_t log_status;
    } state;
    /* Param commands */
    union {
        GS_ADCS_Config_sc_t sc;
        GS_ADCS_Config_sv_t sv;
        GS_ADCS_Config_kf_t kf;
        GS_ADCS_Config_ctrl1_t ctrl1;
        GS_ADCS_Config_ctrl2_t ctrl2;
        GS_ADCS_Config_bdot_t bdot;
        GS_ADCS_Config_sensor_t sensor;
        GS_ADCS_Config_act_t act;
        GS_ADCS_Config_work_t work;
    } param;

    /* BDOT commands */
    union {
      adcs_bdot_data_t get_tumblerate;
    } bdot;
    /* Actuator commands */
    union {
      adcs_act_get_torquers_t get_torquers;
    } actuator;    /* Sensor commands */
    union {
      adcs_sensor_get_magnetometer_t get_magnetometer;
      adcs_sensor_get_sunsensor_t get_sunsensor;
      adcs_sensor_get_gyro_t get_gyro;
      adcs_sensor_get_temperature_t get_temperatures;
    } sensor;
    /* UKF commands */
     union {
    	adcs_ukf_get_data_t get_data;
    	adcs_ukf_get_measurements_t get_measurements;
    	adcs_ukf_get_state_t get_state;
    	adcs_ukf_get_filtmeasurements_t get_filtmeasurements;
     } ukf;
     /* matrix commands */
     union {
       	 adcs_matrix_size_t matrix_size;
       	 adcs_matrix_part_t matrix_part;
       	 adcs_matrix_type_t matrix_type;
       	 adcs_matrix_getpart_t matrix_getpart;
     } matrix;
     /* Tle commands */
     union {
     } tle;
     /* Ephemeris commands */
     union {
    	 GS_ADCS_TLE_t tle;
    	 adcs_ephem_scpos_t get_scpos;
    	 adcs_ephem_scvel_t get_scvel;
    	 adcs_ephem_sunpos_t get_sunpos;
    	 adcs_ephem_mag_t get_mag;
    	 adcs_ephem_q_i_o_t get_qio;
    	 adcs_ephem_q_i_e_t get_qie;
    	 adcs_ephem_eclipsetime_t get_eclipsetime;
    } ephem;
    union {
      GS_ADCS_LQR_t ctrldata;
      GS_ADCS_flip_t flipdata;
    } ctrl;
  };
} adcs_server_packet_t;

#define ADCS_SERVER_PACKET_SIZE(t)			\
  (sizeof(((adcs_server_packet_t *) 0)->cmd)   +	\
   sizeof(((adcs_server_packet_t *) 0)->reply) +	\
   sizeof(t))

#endif /* ADCS_CLIENT_TYPES_H_ */
