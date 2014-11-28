/*
 ============================================================================
 Name        : GS_ADCS_BDOT.h
 Author      : Lars Alminde
 Version     :
 Copyright   : GomSpace 2010
 Description : BDOT
 ============================================================================
 */

#ifndef GS_ADCS_BDOT_H_
#define GS_ADCS_BDOT_H_


typedef struct {
  bool_t Detumbled;                 //Are we detumbled or not ?
  adcs_float_t ts;                  //Sample time
  adcs_float_t BfieldOld[3];        //filtered data last iteratio 
  adcs_float_t BfieldOld2[3];       //filtered data last iteration
  adcs_float_t	RateOld;	    //old value for rate norm estimate
  adcs_float_t	RateOld2;	    //old value for rate norm estimate
  unsigned long int count;          //call counter for bias actuation selection
  unsigned int initialcount;        //call counter used for initial 
  // Filter constants 
  adcs_float_t filterconstant_fast;
  adcs_float_t filterconstant_slow;
  adcs_float_t filterconstant_fast2;
  adcs_float_t filterconstant_slow2;
  // Bdot output
  adcs_float_t torquerDutyCycle[3];
  bool_t magvalid;
  bool_t last_valid;
} GS_ADCS_BDOT_Data_t;

// Force bdot to go into detumbled
void GS_ADCS_BDOT_Force_Detumbled(GS_ADCS_Config_t* cf, GS_ADCS_BDOT_Data_t *bdot_data);
// Init the Bdot algorithm
void GS_ADCS_BDOT_Init(GS_ADCS_Config_t* cf, GS_ADCS_BDOT_Data_t *bdot_data);
// Update bdot
int GS_ADCS_BDOT(GS_ADCS_Config_t* cf,  GS_ADCS_MeasurementsData_t *sensors, GS_ADCS_OutputData_t* out, GS_ADCS_BDOT_Data_t *bdot_data);


#endif /* GS_ADCS_BDOT_H_ */
