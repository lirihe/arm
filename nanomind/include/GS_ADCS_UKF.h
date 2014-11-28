/*
 ============================================================================
 Name        : GS_ADCS_UKF.h
 Author      : Morten Bisgaard / JAL
 Version     : 1.0
 Copyright   : GomSpace 2011
 Description : Unscented Kalman Filter
 ============================================================================
*/

#ifndef GS_ACDS_UKF_H
#define GS_ACDS_UKF_H

//#include "../ephemeris/GS_ADCS_EPHEM.h"
#include "GS_ADCS.h"



//#define SCALED_SYMMETRIC_UKF
#define SPHERICAL_SIMPLEX_UKF


// Standard UKF parameters
#ifdef SCALED_SYMMETRIC_UKF
#define SSP

#define NUM_SP (UKF_NUM_ERR*2+1)
//#define ALPHA_UKF  0.1
#define ALPHA_UKF  1.7321
#define BETA_UKF   2.0
#define KAPPA_UKF  0.0
#define LAMBDA_UKF  (ALPHA_UKF*ALPHA_UKF*(UKF_NUM_ERR+KAPPA_UKF)-UKF_NUM_ERR)
#endif


#ifdef SPHERICAL_SIMPLEX_UKF

#define NUM_SP (UKF_NUM_ERR+2)
#define ALPHA_UKF   0.1
#define BETA_UKF    2.0
#define WEIGHT0_UKF 0.5
void calculateSphericalSimplexWeights (adcs_float_t *sigma, adcs_float_t *weights);
#endif

// Threshold for when to think that the sat. is in eclipse.
#define SUN_NORM_THRESHOLD .50



// UKF data struct
//---------------------------------------------------------------------------
// NOTE: All UKF vars are declared of MAX length, but may be used with smaller length depending on config.
typedef struct {
  // Statevector: 
  // [q1 q2 q3 q4 w1 w2 w3 bm1 bm2 bm3 bg1 bg2 bg3 tau1 tau2 tau2]
  // [0  1  2  3  4  5  6  7   8   9   10  11  12  13   14   15  ]
  adcs_float_t EstimatedState[UKF_MAX_NUM_X];      // State vector estimated by UKF (X)
  adcs_float_t Pest[UKF_MAX_NUM_ERR*UKF_MAX_NUM_ERR];  // Covariance matrix
  adcs_float_t K[UKF_MAX_NUM_ERR*UKF_MAX_NUM_Z];       // Kalman Gain
  adcs_float_t Qm[UKF_MAX_NUM_ERR*UKF_MAX_NUM_ERR];    // Model noise
  adcs_float_t Rm[UKF_MAX_NUM_Z*UKF_MAX_NUM_Z];        // Sensor noise
  adcs_float_t Sigma[MAX_NUM_SP*UKF_MAX_NUM_ERR];      // Sigma matrix for SSUKF
  adcs_float_t Weights[MAX_NUM_SP+1];              // Weight vector for UKF
  adcs_float_t *Xpred;                         // Pointer to predicted state
  adcs_float_t *Zpred;                         // Pointer to predicted measurement
  adcs_float_t Ts;                             // Timestep for UKF
  bool_t KFconverged;                          //
  long int Iterations;                         // Number of iterations by the UKF
  adcs_float_t sunsensorMax[6];                // Current maximum value for the sunsensor max tracking algorithm
  bool_t InEclipse;                            // UKF eclipse detection
} GS_ADCS_UKF_Data_t;

// UKF measurement struct
//---------------------------------------------------------------------------
typedef struct {
  adcs_float_t Measurement[UKF_MAX_NUM_SENS];    // Sensor vector 
  adcs_float_t MeasurementFilt[UKF_MAX_NUM_Z]; // Filtered sensorVector (Z) [sun1 sun2 sun3 mag1 mag2 mag3 g1 g2 g3]
  adcs_float_t MeasurementOld[UKF_MAX_NUM_Z]; // Last run measurement vector (used to
  bool_t Enable[UKF_MAX_NUM_SENS];         // Enable vector for measurents (set to zero to disable sensor)
} GS_ADCS_UKF_Measurement_t;

// UKF input struct
//---------------------------------------------------------------------------
typedef struct {
  adcs_float_t CtrlTorques[3];    // control torque vector 
} GS_ADCS_UKF_Inputs_t;



// Functions
//---------------------------------------------------------------------------

unsigned int GS_ADCS_UKF_Get_NUM_X();
unsigned int GS_ADCS_UKF_Get_NUM_SENS();
unsigned int GS_ADCS_UKF_Get_NUM_Z();
unsigned int GS_ADCS_UKF_Get_NUM_ERR();

// Update Filter
/**
 * Updage the UKF with current measurements
 * @param UKF_data
 * @param UKF_meas
 * @param ephem
 * @param config
 * @return 0 - no error
 * 		   1 - Previous estiamated state resultated in a NaN
 * 		   2 - One of the measurements is NaN
 */
int GS_ADCS_UKF_Update(GS_ADCS_UKF_Data_t *UKF_data, GS_ADCS_UKF_Measurement_t *UKF_meas,  GS_ADCS_Ephem_t *ephem, GS_ADCS_Config_t *config,adcs_float_t *ctrlsignals);

/**
 * Check sizes of kalman filter arrays.
 * @return 0=no error, -1=Error in array sizes
 */
int GS_ADCS_UKF_CheckSizes();

/**
 * Initialize filter with initial states, covariance etc
 * @param UKF_data
 * @param UKF_meas
 * @param ephem
 * @return 0=no error, -3=Error in array sizes 
 */
int GS_ADCS_UKF_Init(GS_ADCS_Config_t *config, GS_ADCS_UKF_Data_t *UKF_data, GS_ADCS_UKF_Measurement_t *UKF_meas, GS_ADCS_Ephem_t *ephem);

/**
 * Wrap into input vector
 * @param dutycycle
 * @param ukf_inputs
 * @param ephem
 * @param ukf_data
 */
void GS_ADCS_UKF_SetInputs(adcs_float_t *dutycycle, GS_ADCS_UKF_Inputs_t *ukf_inputs, GS_ADCS_Ephem_t *ephem, GS_ADCS_WorkData_t *ukf_workdata, GS_ADCS_Config_t *config);
/**
 * Wrap into measurement vector
 * @param UKF_meas
 * @param rawDataPool
 */
void GS_ADCS_UKF_SetMeasurements(GS_ADCS_UKF_Measurement_t *ukf_measurements, GS_ADCS_MeasurementsData_t *rawDataPool);

/**
 * Wrap into output vector
 * @param outputdata
 * @param ukf_data
 */
void GS_ADCS_UKF_SetOutputs(GS_ADCS_OutputData_t *outputdata, GS_ADCS_UKF_Data_t *ukf_data);

// Invalidate old measurement (enable = false)
void GS_ADCS_UKF_InvalidOldMeas(GS_ADCS_UKF_Measurement_t *meas);
// Zero all vectors
void   GS_ADCS_UKF_Zero(GS_ADCS_UKF_Data_t *UKF_data, GS_ADCS_UKF_Measurement_t *UKF_meas);
// Prefilter, set enable vector etc.
void   GS_ADCS_UKF_Prefilter(GS_ADCS_UKF_Data_t *UKF_data, GS_ADCS_UKF_Measurement_t *UKF_meas, GS_ADCS_Config_t *config);

// Internal functions
//---------------------------------------------------------------------------

// Euler integration of model
void GS_ADCS_UKF_ModelInt (adcs_float_t *spXo, adcs_float_t *spXi, unsigned int sp, adcs_float_t Ts, GS_ADCS_Config_t *config, adcs_float_t *u);
// Actual model
void GS_ADCS_UKF_Model(adcs_float_t *Xdot, adcs_float_t *X,GS_ADCS_Config_t *config,  adcs_float_t *u);
// Sensor model
void GS_ADCS_UKF_SensorModel(adcs_float_t *Z, adcs_float_t *X, bool_t *enable, GS_ADCS_Ephem_t *ephem, GS_ADCS_Config_t *config);

void calculateSigmaPointsWeights (adcs_float_t *sigma, adcs_float_t *weight);
void calculateSigmaPoints (adcs_float_t *point, adcs_float_t *weight,  adcs_float_t *x, adcs_float_t *P, adcs_float_t *SPscaling, adcs_float_t *Sigma);
void calculate_Pxx (adcs_float_t *Pxx, adcs_float_t *Xpts, adcs_float_t *X, adcs_float_t *WSP, adcs_float_t *Qm);
void calculate_Pzz (adcs_float_t *Pzz, adcs_float_t *Zpts, adcs_float_t *Z, adcs_float_t *WSP, adcs_float_t *Rm);
void calculate_Pxz (adcs_float_t *Pxz, adcs_float_t *Xpts, adcs_float_t *X, adcs_float_t *Zpts, adcs_float_t *Z, adcs_float_t *WSP);

quaternion_t toQuaternion(adcs_float_t *q123);
#endif
