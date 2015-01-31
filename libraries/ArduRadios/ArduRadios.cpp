/*
 * ArduRadios.cpp
 *
 *  Created on: Jan 18, 2014
 *      Author: Rancuret
 */

#include "ArduRadios.h"

// definitions
ArduRadio ArduRadios; // definition of the ArduRadios class

extern "C" {
static volatile uint16_t timer0count = 0;                    // Initial value for Latest count value for timer corresponding to radio channel 0
static volatile uint16_t timer1count = 0;                    // Initial value for Latest count value for timer corresponding to radio channel 1
static volatile uint16_t timer2count = 0;                    // Initial value for Latest count value for timer corresponding to radio channel 2
static volatile uint16_t timer3count = 0;                    // Initial value for Latest count value for timer corresponding to radio channel 3
static volatile uint16_t timer0diff  = ROLL_INITIAL * 2;     // Initial value for Difference in latest timer values corresponding to radio channel 0
static volatile uint16_t timer1diff  = PITCH_INITIAL * 2;    // Initial value for Difference in latest timer values corresponding to radio channel 1
static volatile uint16_t timer2diff  = THROTTLE_INITIAL * 2; // Initial value for Difference in latest timer values corresponding to radio channel 2
static volatile uint16_t timer3diff  = RUDDER_INITIAL * 2;   // Initial value for Difference in latest timer values corresponding to radio channel 3
static volatile boolean  ch0_read    = false;                // Initial value for Latest read value of radio input pin for channel 0
static volatile boolean  ch1_read    = false;                // Initial value for Latest read value of radio input pin for channel 1
static volatile boolean  ch2_read    = false;                // Initial value for Latest read value of radio input pin for channel 2
static volatile boolean  ch3_read    = false;                // Initial value for Latest read value of radio input pin for channel 3
}
// ArduRadio public functions

/*
 * Class:		ArduRadio
 * Function:	ArduRadio()
 * Scope:		Public
 * Arguments:	None
 * Description:	Constructor for ArduRadio object
 */
ArduRadio :: ArduRadio ( ) {
  radio_in [ CH_ROLL ]          = ROLL_INITIAL;                 // Initial value for Roll channel of radio input
  radio_in [ CH_PITCH ]         = PITCH_INITIAL;                // Initial value for Pitch channel of radio input
  radio_in [ CH_THROTTLE ]      = THROTTLE_INITIAL;             // Initial value for Throttle channel of radio input
  radio_in [ CH_RUDDER ]        = RUDDER_INITIAL;               // Initial value for Rudder channel of radio input
  radio_offsets [ CH_ROLL ]     = 0;                            // Initial value for Offset values for roll radio channel
  radio_offsets [ CH_PITCH ]    = 0;                            // Initial value for Offset values for pitch radio channel
  radio_offsets [ CH_THROTTLE ] = 0;                            // Initial value for Offset values for throttle radio channel
  radio_offsets [ CH_RUDDER ]   = 0;                            // Initial value for Offset values for rudder radio channel
  timer0count                   = 0;                            // Initial value for Latest count value for timer corresponding to radio channel 0
  timer1count                   = 0;                            // Initial value for Latest count value for timer corresponding to radio channel 1
  timer2count                   = 0;                            // Initial value for Latest count value for timer corresponding to radio channel 2
  timer3count                   = 0;                            // Initial value for Latest count value for timer corresponding to radio channel 3
  timer0diff                    = radio_in [ CH_ROLL ] * 2;     // Initial value for Difference in latest timer values corresponding to radio channel 0
  timer1diff                    = radio_in [ CH_PITCH ] * 2;    // Initial value for Difference in latest timer values corresponding to radio channel 1
  timer2diff                    = radio_in [ CH_THROTTLE ] * 2; // Initial value for Difference in latest timer values corresponding to radio channel 2
  timer3diff                    = radio_in [ CH_RUDDER ] * 2;   // Initial value for Difference in latest timer values corresponding to radio channel 3
  ch0_read                      = false;                        // Initial value for Latest read value of radio input pin for channel 0
  ch1_read                      = false;                        // Initial value for Latest read value of radio input pin for channel 1
  ch2_read                      = false;                        // Initial value for Latest read value of radio input pin for channel 2
  ch3_read                      = false;                        // Initial value for Latest read value of radio input pin for channel 3
  use_filters                   = false;                        // Initial value for Logic value which determines whether or not to use any filters
  filtcoeffs [ CH_ROLL ]        = 0;                            // Initial value for Filter coefficient for roll channel.  This number, which must lie in the range [0,31], determines how much filtering to apply.  The larger the number, the lower the effective filter cutoff frequency.
  filtcoeffs [ CH_PITCH ]       = 0;                            // Initial value for filter coefficient for pitch channel.  This number, which must lie in the range [0,31], determines how much filtering to apply.  The larger the number, the lower the effective filter cutoff frequency.
  filtcoeffs [ CH_THROTTLE ]    = 0;                            // Initial value for filter coefficient for throttle channel.  This number, which must lie in the range [0,31], determines how much filtering to apply.  The larger the number, the lower the effective filter cutoff frequency.
  filtcoeffs [ CH_RUDDER ]      = 0;                            // Initial value for filter coefficient for rudder channel.  This number, which must lie in the range [0,31], determines how much filtering to apply.  The larger the number, the lower the effective filter cutoff frequency.
  initialized                   = false;                        // Initial value for Logic value which tells whether or not the Radios class instance has been properly initialized.
  return;
} // end of ArduRadio Constructor Function

/*
 * Class:		ArduRadio
 * Function:	groundInitNoFilt()
 * Scope:		Public
 * Arguments:	None
 * Description:	Ground Initialization function for radios without filters.
 *                              The Ground initialization will automatically calculate radio
 *                              offsets to produce default values.  This sets the radios up
 *                              with no filtering.
 */
uint8_t ArduRadio :: groundInitNoFilt ( void ) {
  uint8_t  rtn                  = RADIO_RTN_SUCCESS;                                                                     // initial return value indicating success.  Will be replaced if failure occurs
  uint16_t lastRadioVals [ 4 ];                                                                                          // holds the latest radio reading taken while performing average calculation
  int32_t  meanRadioVals [ 4 ]  = { ROLL_INITIAL << 4, PITCH_INITIAL << 4, THROTTLE_INITIAL << 4, RUDDER_INITIAL << 4 }; // holds the mean value of radio readings so far times 16 (for better precision)
  int32_t  deltaRadioVals [ 4 ] = { 0, 0, 0, 0 };                                                                        // holds the difference between latest and mean readings times 16 (for better precision)
  uint16_t count;                                                                                                        // counter for loop which takes radio readings

  use_filters = false; // set flag indicating filters are not used

  pinMode ( 2,  INPUT ); // PD2 - INT0     - Rudder in             - INPUT Aileron
  pinMode ( 3,  INPUT ); // PD3 - INT1     - Elevator in             - INPUT Elevator
  pinMode ( 11, INPUT ); // PB3 - MOSI/OC2 - INPUT Rudder
  pinMode ( 13, INPUT ); // PB5 - SCK    - Yellow LED pin            - INPUT Throttle


  // Set Up Timer 1
  TCCR1A = ( ( 1 << WGM11 ) | ( 1 << COM1B1 ) | ( 1 << COM1A1 ) ); // Fast PWM: ICR1=TOP, OCR1x=BOTTOM,TOV1=TOP
  TCCR1B = ( 1 << WGM13 ) | ( 1 << WGM12 ) | ( 1 << CS11 );        // Clock scaler = 8, 2,000,000 counts per second
  OCR1A  = 3000;                                                   // Rudder  - multiply your value * 2; for example 3000 = 1500 = 45°; 4000 = 2000 = 90°
  OCR1B  = 3000;                                                   // Elevator
  ICR1   = TIMERMAXCNT;                                            // 50hz freq...Datasheet says  (system_freq/prescaler)/target frequency. So (16000000hz/8)/50hz = 40000,

  // enable pin change interrupt on PB5 (digital pin 11,13)
  PCMSK0 = _BV ( PCINT3 ) | _BV ( PCINT5 );

  // enable pin change interrupt on PD2,PD3 (digital pin 2,3)
  PCMSK2 = _BV ( PCINT18 ) | _BV ( PCINT19 );

  // enable pin change interrupt 2 - PCINT23..16
  PCICR |= _BV ( PCIE2 );

  // enable pin change interrupt 0 -  PCINT7..0
  PCICR |= _BV ( PCIE0 );


  for ( count = 1; count <= GNDINIT_NUMREADS; count++ )
  {
    read_radio ( lastRadioVals ); // read the radios and store the readings in the lastRadioVals array

    deltaRadioVals [ CH_ROLL ]     = ( ( (int32_t) lastRadioVals [ CH_ROLL ] ) << 4 ) - meanRadioVals [ CH_ROLL ];         // calculate delta between roll channel reading and mean.  Left shift by four (multiply by 16) will give better precision when dividing this number by the number of counts.
    deltaRadioVals [ CH_PITCH ]    = ( ( (int32_t) lastRadioVals [ CH_PITCH ] ) << 4 ) - meanRadioVals [ CH_PITCH ];       // calculate delta between pitch channel reading and mean.  Left shift by four (multiply by 16) will give better precision when dividing this number by the number of counts.
    deltaRadioVals [ CH_THROTTLE ] = ( ( (int32_t) lastRadioVals [ CH_THROTTLE ] ) << 4 ) - meanRadioVals [ CH_THROTTLE ]; // calculate delta between throttle channel reading and mean.  Left shift by four (multiply by 16) will give better precision when dividing this number by the number of counts.
    deltaRadioVals [ CH_RUDDER ]   = ( ( (int32_t) lastRadioVals [ CH_RUDDER ] ) << 4 ) - meanRadioVals [ CH_RUDDER ];     // calculate delta between rudder channel reading and mean.  Left shift by four (multiply by 16) will give better precision when dividing this number by the number of counts.

    meanRadioVals [ CH_ROLL ]     += ( deltaRadioVals [ CH_ROLL ] / count );     // divide delta by the number of times a reading has been taken and add this to the mean.
    meanRadioVals [ CH_PITCH ]    += ( deltaRadioVals [ CH_PITCH ] / count );    // divide delta by the number of times a reading has been taken and add this to the mean.
    meanRadioVals [ CH_THROTTLE ] += ( deltaRadioVals [ CH_THROTTLE ] / count ); // divide delta by the number of times a reading has been taken and add this to the mean.
    meanRadioVals [ CH_RUDDER ]   += ( deltaRadioVals [ CH_RUDDER ] / count );   // divide delta by the number of times a reading has been taken and add this to the mean.

    delay ( GNDINIT_DELAY_MS );
  }

  radio_offsets [ CH_ROLL ]     = ROLL_INITIAL - ( meanRadioVals [ CH_ROLL ] >> 4 );         // Set offset for roll axis to achieve a new average right at the desired initial value.  right shifting by four (divide by 16) corrects the scaling factor added above in the delta
  radio_offsets [ CH_PITCH ]    = PITCH_INITIAL - ( meanRadioVals [ CH_PITCH ] >> 4 );       // Set offset for roll axis to achieve a new average right at the desired initial value.  right shifting by four (divide by 16) corrects the scaling factor added above in the delta
  radio_offsets [ CH_THROTTLE ] = THROTTLE_INITIAL - ( meanRadioVals [ CH_THROTTLE ] >> 4 ); // Set offset for roll axis to achieve a new average right at the desired initial value.  right shifting by four (divide by 16) corrects the scaling factor added above in the delta
  radio_offsets [ CH_RUDDER ]   = RUDDER_INITIAL - ( meanRadioVals [ CH_RUDDER ] >> 4 );     // Set offset for roll axis to achieve a new average right at the desired initial value.  right shifting by four (divide by 16) corrects the scaling factor added above in the delta

  if ( rtn == RADIO_RTN_SUCCESS ) // if initialization completed succesfully
    initialized = true;           // set flag indicating initializtion is complete

  return rtn;
} // end of groundInitNoFilt()

/*
 * Class:		ArduRadio
 * Function:	groundInitFilt()
 * Scope:		Public
 * Arguments:	uint8_t *filtCoeffs[4] - pointer to an array containing four
 *                                                                               filter coefficient values.  Each
 *                                                                               value must be between 0 and 31.
 *                                                                               Larger numbers will have a slower
 *                                                                               filtered response.
 * Description:	Ground Initialization function for radios with filters.  The
 *                              Ground initialization will automatically calculate radio
 *                              offsets to produce default values.  This sets the radios up
 *                               with specified filter coefficients.
 */
uint8_t ArduRadio :: groundInitFilt ( uint8_t *filtCoeffs ) {
  uint8_t  rtn                  = RADIO_RTN_SUCCESS;                                                                     // initial return value indicating success.  Will be replaced if failure occurs
  uint16_t lastRadioVals [ 4 ];                                                                                          // holds the latest radio reading taken while performing average calculation
  int32_t  meanRadioVals [ 4 ]  = { ROLL_INITIAL << 4, PITCH_INITIAL << 4, THROTTLE_INITIAL << 4, RUDDER_INITIAL << 4 }; // holds the mean value of radio readings so far times 16 (for better precision)
  int32_t  deltaRadioVals [ 4 ] = { 0, 0, 0, 0 };                                                                        // holds the difference between latest and mean readings times 16 (for better precision)
  uint16_t count;                                                                                                        // counter for loop which takes radio readings

  if ( filtCoeffs [ CH_ROLL ] < 32 ) // check to ensure filter coefficient is less than 32
  {
    filtcoeffs [ CH_ROLL ] = filtCoeffs [ CH_ROLL ]; // store coefficient
  }
  else
  {
    rtn = RADIO_RTN_FILTERR; // if invalid coefficient was specified, return error code
  }
  if ( filtCoeffs [ CH_PITCH ] < 32 ) // check to ensure filter coefficient is less than 32
  {
    filtcoeffs [ CH_PITCH ] = filtCoeffs [ CH_PITCH ]; // store coefficient
  }
  else
  {
    rtn = RADIO_RTN_FILTERR; // if invalid coefficient was specified, return error code
  }
  if ( filtCoeffs [ CH_THROTTLE ] < 32 ) // check to ensure filter coefficient is less than 32
  {
    filtcoeffs [ CH_THROTTLE ] = filtCoeffs [ CH_THROTTLE ]; // store coefficient
  }
  else
  {
    rtn = RADIO_RTN_FILTERR; // if invalid coefficient was specified, return error code
  }
  if ( filtCoeffs [ CH_RUDDER ] < 32 ) // check to ensure filter coefficient is less than 32
  {
    filtcoeffs [ CH_RUDDER ] = filtCoeffs [ CH_RUDDER ]; // store coefficient
  }
  else
  {
    rtn = RADIO_RTN_FILTERR; // if invalid coefficient was specified, return error code
  }

  pinMode ( 2,  INPUT ); // PD2 - INT0     - Rudder in             - INPUT Aileron
  pinMode ( 3,  INPUT ); // PD3 - INT1     - Elevator in             - INPUT Elevator
  pinMode ( 11, INPUT ); // PB3 - MOSI/OC2 - INPUT Rudder
  pinMode ( 13, INPUT ); // PB5 - SCK    - Yellow LED pin            - INPUT Throttle


  // Set Up Timer 1
  TCCR1A = ( ( 1 << WGM11 ) | ( 1 << COM1B1 ) | ( 1 << COM1A1 ) ); // Fast PWM: ICR1=TOP, OCR1x=BOTTOM,TOV1=TOP
  TCCR1B = ( 1 << WGM13 ) | ( 1 << WGM12 ) | ( 1 << CS11 );        // Clock scaler = 8, 2,000,000 counts per second
  OCR1A  = 3000;                                                   // Rudder  - multiply your value * 2; for example 3000 = 1500 = 45°; 4000 = 2000 = 90°
  OCR1B  = 3000;                                                   // Elevator
  ICR1   = TIMERMAXCNT;                                            // 50hz freq...Datasheet says  (system_freq/prescaler)/target frequency. So (16000000hz/8)/50hz = 40000,

  // enable pin change interrupt on PB5 (digital pin 11,13)
  PCMSK0 = _BV ( PCINT3 ) | _BV ( PCINT5 );

  // enable pin change interrupt on PD2,PD3 (digital pin 2,3)
  PCMSK2 = _BV ( PCINT18 ) | _BV ( PCINT19 );

  // enable pin change interrupt 2 - PCINT23..16
  PCICR |= _BV ( PCIE2 );

  // enable pin change interrupt 0 -  PCINT7..0
  PCICR      |= _BV ( PCIE0 );

  use_filters = false; // start without filters, just while doing calibration

  for ( count = 1; count <= GNDINIT_NUMREADS; count++ )
  {
    read_radio ( lastRadioVals ); // read the radios and store the readings in the lastRadioVals array

    deltaRadioVals [ CH_ROLL ]     = ( ( (int32_t) lastRadioVals [ CH_ROLL ] ) << 4 ) - meanRadioVals [ CH_ROLL ];         // calculate delta between roll channel reading and mean.  Left shift by four (multiply by 16) will give better precision when dividing this number by the number of counts.
    deltaRadioVals [ CH_PITCH ]    = ( ( (int32_t) lastRadioVals [ CH_PITCH ] ) << 4 ) - meanRadioVals [ CH_PITCH ];       // calculate delta between pitch channel reading and mean.  Left shift by four (multiply by 16) will give better precision when dividing this number by the number of counts.
    deltaRadioVals [ CH_THROTTLE ] = ( ( (int32_t) lastRadioVals [ CH_THROTTLE ] ) << 4 ) - meanRadioVals [ CH_THROTTLE ]; // calculate delta between throttle channel reading and mean.  Left shift by four (multiply by 16) will give better precision when dividing this number by the number of counts.
    deltaRadioVals [ CH_RUDDER ]   = ( ( (int32_t) lastRadioVals [ CH_RUDDER ] ) << 4 ) - meanRadioVals [ CH_RUDDER ];     // calculate delta between rudder channel reading and mean.  Left shift by four (multiply by 16) will give better precision when dividing this number by the number of counts.

    meanRadioVals [ CH_ROLL ]     += ( deltaRadioVals [ CH_ROLL ] / count );     // divide delta by the number of times a reading has been taken and add this to the mean.
    meanRadioVals [ CH_PITCH ]    += ( deltaRadioVals [ CH_PITCH ] / count );    // divide delta by the number of times a reading has been taken and add this to the mean.
    meanRadioVals [ CH_THROTTLE ] += ( deltaRadioVals [ CH_THROTTLE ] / count ); // divide delta by the number of times a reading has been taken and add this to the mean.
    meanRadioVals [ CH_RUDDER ]   += ( deltaRadioVals [ CH_RUDDER ] / count );   // divide delta by the number of times a reading has been taken and add this to the mean.

    delay ( GNDINIT_DELAY_MS );
  }

  radio_offsets [ CH_ROLL ]     = ROLL_INITIAL - ( meanRadioVals [ CH_ROLL ] >> 4 );         // Set offset for roll axis to achieve a new average right at the desired initial value.  right shifting by four (divide by 16) corrects the scaling factor added above in the delta
  radio_offsets [ CH_PITCH ]    = PITCH_INITIAL - ( meanRadioVals [ CH_PITCH ] >> 4 );       // Set offset for roll axis to achieve a new average right at the desired initial value.  right shifting by four (divide by 16) corrects the scaling factor added above in the delta
  radio_offsets [ CH_THROTTLE ] = THROTTLE_INITIAL - ( meanRadioVals [ CH_THROTTLE ] >> 4 ); // Set offset for roll axis to achieve a new average right at the desired initial value.  right shifting by four (divide by 16) corrects the scaling factor added above in the delta
  radio_offsets [ CH_RUDDER ]   = RUDDER_INITIAL - ( meanRadioVals [ CH_RUDDER ] >> 4 );     // Set offset for roll axis to achieve a new average right at the desired initial value.  right shifting by four (divide by 16) corrects the scaling factor added above in the delta

  if ( rtn == RADIO_RTN_SUCCESS ) // if no problems occurred while setting filter coefficient
    use_filters = true;           // set flag indicating filters are used

  if ( rtn == RADIO_RTN_SUCCESS ) // if initialization completed succesfully
    initialized = true;           // set flag indicating initializtion is complete

  return rtn;
} // end of groundInitFilt()

/*
 * Class:		ArduRadio
 * Function:	airInitNoFilt()
 * Scope:		Public
 * Arguments:	None
 * Description:	Air Initialization function for radios without filters.  The
 *                              Air initialization will use default (zero) values for
 *                              offsets.  This sets the radios up with no filtering.
 */
uint8_t ArduRadio :: airInitNoFilt ( void ) {
  uint8_t rtn = RADIO_RTN_SUCCESS; // initial return value indicating success.  Will be replaced if failure occurs

  use_filters = false; // set flag indicating filters are not used

  pinMode ( 2,  INPUT ); // PD2 - INT0     - Rudder in             - INPUT Aileron
  pinMode ( 3,  INPUT ); // PD3 - INT1     - Elevator in             - INPUT Elevator
  pinMode ( 11, INPUT ); // PB3 - MOSI/OC2 - INPUT Rudder
  pinMode ( 13, INPUT ); // PB5 - SCK    - Yellow LED pin            - INPUT Throttle


  // Set Up Timer 1
  TCCR1A = ( ( 1 << WGM11 ) | ( 1 << COM1B1 ) | ( 1 << COM1A1 ) ); // Fast PWM: ICR1=TOP, OCR1x=BOTTOM,TOV1=TOP
  TCCR1B = ( 1 << WGM13 ) | ( 1 << WGM12 ) | ( 1 << CS11 );        // Clock scaler = 8, 2,000,000 counts per second
  OCR1A  = 3000;                                                   // Rudder  - multiply your value * 2; for example 3000 = 1500 = 45°; 4000 = 2000 = 90°
  OCR1B  = 3000;                                                   // Elevator
  ICR1   = TIMERMAXCNT;                                            // 50hz freq...Datasheet says  (system_freq/prescaler)/target frequency. So (16000000hz/8)/50hz = 40000,

  // enable pin change interrupt on PB5 (digital pin 11,13)
  PCMSK0 = _BV ( PCINT3 ) | _BV ( PCINT5 );

  // enable pin change interrupt on PD2,PD3 (digital pin 2,3)
  PCMSK2 = _BV ( PCINT18 ) | _BV ( PCINT19 );

  // enable pin change interrupt 2 - PCINT23..16
  PCICR |= _BV ( PCIE2 );

  // enable pin change interrupt 0 -  PCINT7..0
  PCICR |= _BV ( PCIE0 );

  if ( rtn == RADIO_RTN_SUCCESS ) // if initialization completed succesfully
    initialized = true;           // set flag indicating initializtion is complete

  return rtn;
} // end of airInitNoFilt()

/*
 * Class:		ArduRadio
 * Function:	airInitFilt()
 * Scope:		Public
 * Arguments:	uint8_t *filtCoeffs[4] - pointer to an array containing four
 *                                                                               filter coefficient values.  Each
 *                                                                               value must be between 0 and 31.
 *                                                                               Larger numbers will have a slower
 *                                                                               filtered response.
 * Description:	Air Initialization function for radios with filters.  The Air
 *                          initialization will use default (zero) values for offsets.
 *                          This sets the radios up with specified filter coefficients.
 */
uint8_t ArduRadio :: airInitFilt ( uint8_t *filtCoeffs ) {
  uint8_t rtn = RADIO_RTN_SUCCESS; // initial return value indicating success.  Will be replaced if failure occurs

  use_filters = false;               // start with filter disabled (will be turned on later)
  if ( filtCoeffs [ CH_ROLL ] < 32 ) // check to ensure filter coefficient is less than 32
  {
    filtcoeffs [ CH_ROLL ] = filtCoeffs [ CH_ROLL ]; // store coefficient
  }
  else
  {
    rtn = RADIO_RTN_FILTERR; // if invalid coefficient was specified, return error code
  }
  if ( filtCoeffs [ CH_PITCH ] < 32 ) // check to ensure filter coefficient is less than 32
  {
    filtcoeffs [ CH_PITCH ] = filtCoeffs [ CH_PITCH ]; // store coefficient
  }
  else
  {
    rtn = RADIO_RTN_FILTERR; // if invalid coefficient was specified, return error code
  }
  if ( filtCoeffs [ CH_THROTTLE ] < 32 ) // check to ensure filter coefficient is less than 32
  {
    filtcoeffs [ CH_THROTTLE ] = filtCoeffs [ CH_THROTTLE ]; // store coefficient
  }
  else
  {
    rtn = RADIO_RTN_FILTERR; // if invalid coefficient was specified, return error code
  }
  if ( filtCoeffs [ CH_RUDDER ] < 32 ) // check to ensure filter coefficient is less than 32
  {
    filtcoeffs [ CH_RUDDER ] = filtCoeffs [ CH_RUDDER ]; // store coefficient
  }
  else
  {
    rtn = RADIO_RTN_FILTERR; // if invalid coefficient was specified, return error code
  }

  pinMode ( 2,  INPUT ); // PD2 - INT0     - Rudder in             - INPUT Aileron
  pinMode ( 3,  INPUT ); // PD3 - INT1     - Elevator in             - INPUT Elevator
  pinMode ( 11, INPUT ); // PB3 - MOSI/OC2 - INPUT Rudder
  pinMode ( 13, INPUT ); // PB5 - SCK    - Yellow LED pin            - INPUT Throttle


  // Set Up Timer 1
  TCCR1A = ( ( 1 << WGM11 ) | ( 1 << COM1B1 ) | ( 1 << COM1A1 ) ); // Fast PWM: ICR1=TOP, OCR1x=BOTTOM,TOV1=TOP
  TCCR1B = ( 1 << WGM13 ) | ( 1 << WGM12 ) | ( 1 << CS11 );        // Clock scaler = 8, 2,000,000 counts per second
  OCR1A  = 3000;                                                   // Rudder  - multiply your value * 2; for example 3000 = 1500 = 45°; 4000 = 2000 = 90°
  OCR1B  = 3000;                                                   // Elevator
  ICR1   = TIMERMAXCNT;                                            // 50hz freq...Datasheet says  (system_freq/prescaler)/target frequency. So (16000000hz/8)/50hz = 40000,

  // enable pin change interrupt on PB5 (digital pin 11,13)
  PCMSK0 = _BV ( PCINT3 ) | _BV ( PCINT5 );

  // enable pin change interrupt on PD2,PD3 (digital pin 2,3)
  PCMSK2 = _BV ( PCINT18 ) | _BV ( PCINT19 );

  // enable pin change interrupt 2 - PCINT23..16
  PCICR |= _BV ( PCIE2 );

  // enable pin change interrupt 0 -  PCINT7..0
  PCICR |= _BV ( PCIE0 );

  if ( rtn == RADIO_RTN_SUCCESS ) // if no problems occurred while setting filter coefficient
    use_filters = true;           // set flag indicating filters are used

  if ( rtn == RADIO_RTN_SUCCESS ) // if initialization completed succesfully
    initialized = true;           // set flag indicating initializtion is complete

  return rtn;
} // end of airInitFilt()

/*
 * Class:		ArduRadio
 * Function:	read_radio()
 * Scope:		Public
 * Arguments:	radiovals	- array to put resulting values
 * Description:	This function reads the radio values, stores
 *                              them in the specified array, and returns success code
 */
uint8_t ArduRadio :: read_radio ( uint16_t *radiovals ) {
  uint8_t  rtn = RADIO_RTN_SUCCESS; // value to return if succesful
  uint16_t tempvalue;               // temporary value used when filtering

  if ( initialized == false )
  {
    rtn |= RADIO_RTN_NOINIT;
  }

  if ( use_filters ) // if filters are used
  {

    // Read and Filter the Roll channel
    tempvalue = ( timer0diff >> 1 ) + radio_offsets [ CH_ROLL ];     // read roll channel
    tempvalue = constrain ( tempvalue, RADIOPWM_MIN, RADIOPWM_MAX ); // constrain roll channel to min and max values
    if ( filtcoeffs [ CH_ROLL ] < 32 )
    {
      radio_in [ CH_ROLL ] = ( ( radio_in [ CH_ROLL ] * filtcoeffs [ CH_ROLL ] ) + ( tempvalue * ( 32 - filtcoeffs [ CH_ROLL ] ) ) ) >> 5; // apply filter to roll channel
    }
    else
    {
      rtn                 |= RADIO_RTN_FILTERR; // set return to indicate filter error
      radio_in [ CH_ROLL ] = tempvalue;         // use unfiltered radio reading
    }

    // Read and Filter the Pitch channel
    tempvalue = ( timer1diff >> 1 ) + radio_offsets [ CH_PITCH ];    // read pitch channel
    tempvalue = constrain ( tempvalue, RADIOPWM_MIN, RADIOPWM_MAX ); // constrain pitch channel to min and max values
    if ( filtcoeffs [ CH_PITCH ] < 32 )
    {
      radio_in [ CH_PITCH ] = ( ( radio_in [ CH_PITCH ] * filtcoeffs [ CH_PITCH ] ) + ( tempvalue * ( 32 - filtcoeffs [ CH_PITCH ] ) ) ) >> 5; // apply filter to pitch channel
    }
    else
    {
      rtn                  |= RADIO_RTN_FILTERR; // set return to indicate filter error
      radio_in [ CH_PITCH ] = tempvalue;         // use unfiltered radio reading
    }

    // Read and Filter the Throttle channel
    tempvalue = ( timer2diff >> 1 ) + radio_offsets [ CH_THROTTLE ]; // read throttle channel
    tempvalue = constrain ( tempvalue, RADIOPWM_MIN, RADIOPWM_MAX ); // constrain throttle channel to min and max values
    if ( filtcoeffs [ CH_THROTTLE ] < 32 )
    {
      radio_in [ CH_THROTTLE ] = ( ( radio_in [ CH_THROTTLE ] * filtcoeffs [ CH_THROTTLE ] ) + ( tempvalue * ( 32 - filtcoeffs [ CH_THROTTLE ] ) ) ) >> 5; // apply filter to throttle channel
    }
    else
    {
      rtn                     |= RADIO_RTN_FILTERR; // set return to indicate filter error
      radio_in [ CH_THROTTLE ] = tempvalue;         // use unfiltered radio reading
    }

    // Read and Filter the Rudder channel
    tempvalue = ( timer3diff >> 1 ) + radio_offsets [ CH_RUDDER ];   // read rudder channel
    tempvalue = constrain ( tempvalue, RADIOPWM_MIN, RADIOPWM_MAX ); // constrain rudder channel to min and max values
    if ( filtcoeffs [ CH_RUDDER ] < 32 )
    {
      radio_in [ CH_RUDDER ] = ( ( radio_in [ CH_RUDDER ] * filtcoeffs [ CH_RUDDER ] ) + ( tempvalue * ( 32 - filtcoeffs [ CH_RUDDER ] ) ) ) >> 5; // apply filter to rudder channel
    }
    else
    {
      rtn                   |= RADIO_RTN_FILTERR; // set return to indicate filter error
      radio_in [ CH_RUDDER ] = tempvalue;         // use unfiltered radio reading
    }

  }
  else // if filters are not used
  {

    // Read the Roll Channel
    tempvalue            = ( timer0diff >> 1 ) + radio_offsets [ CH_ROLL ];     // read roll channel
    radio_in [ CH_ROLL ] = constrain ( tempvalue, RADIOPWM_MIN, RADIOPWM_MAX ); // constrain and use unfiltered radio reading

    // Read the Pitch Channel
    tempvalue             = ( timer1diff >> 1 ) + radio_offsets [ CH_PITCH ];    // read pitch channel
    radio_in [ CH_PITCH ] = constrain ( tempvalue, RADIOPWM_MIN, RADIOPWM_MAX ); // constrain and use unfiltered radio reading

    // Read the Throttle Channel
    tempvalue                = ( timer2diff >> 1 ) + radio_offsets [ CH_THROTTLE ]; // read throttle channel
    radio_in [ CH_THROTTLE ] = constrain ( tempvalue, RADIOPWM_MIN, RADIOPWM_MAX ); // constrain and use unfiltered radio reading

    // Read the Rudder Channel
    tempvalue              = ( timer3diff >> 1 ) + radio_offsets [ CH_RUDDER ];   // read rudder channel
    radio_in [ CH_RUDDER ] = constrain ( tempvalue, RADIOPWM_MIN, RADIOPWM_MAX ); // constrain and use unfiltered radio reading

  }

  if ( rtn == RADIO_RTN_SUCCESS ) // if no failures occurred
  {
    radiovals [ CH_ROLL ]     = radio_in [ CH_ROLL ];     // store roll channel value
    radiovals [ CH_PITCH ]    = radio_in [ CH_PITCH ];    // store pitch channel value
    radiovals [ CH_THROTTLE ] = radio_in [ CH_THROTTLE ]; // store throttle channel value
    radiovals [ CH_RUDDER ]   = radio_in [ CH_RUDDER ];   // store rudder channel value
  }

  return rtn;
} // end of read_radio()


extern "C" {

/*
 * Class:		ArduRadio
 * Function:	Timer_0_1_ISR()
 * Scope:		Public
 * Arguments:	None
 * Description:	Interrupt Service routine for timers 0 and 1
 *                              This function is used to read the timer values
 *                              and record the differences so that the channel
 *                              0 and 1 PWM values can be interpreted.
 */
static void Timer_0_1_ISR ( void ) {
  uint16_t cnt = TCNT1; // read the timer value

  // Check for rising or falling edge on pin 2 (radio channel 0)
  if ( PIND & B00000100 ) // if radio channel 0 (pin 2) is high
  {
    if ( ch0_read == false ) // if radio channel 0 (pin 2) is high and its read value was low last time this interrupt was called (rising edge occurred)
    {
      ch0_read    = true; // mark that the value is read high this time
      timer0count = cnt;  // record the timer count value - to be used later on the falling edge.
    }
  }
  else if ( ch0_read == true ) // if radio channel 0 (pin 2) is Low and its read value was high last time this interrupt was called (falling edge occurred)
  {
    ch0_read = false;                                    // mark that the value is read low this time
    if ( cnt < timer0count )                             // Timer reset during the read of this pulse
      timer0diff = ( cnt + TIMERMAXCNT - timer0count );  // track the timer difference between rising and falling edges
    else
      timer0diff = ( cnt - timer0count ); // track the timer difference between rising and falling edges
  }

  // Check for rising or falling edge on pin 3 (radio channel 1)
  if ( PIND & B00001000 ) // if radio channel 1 (pin 3) is high
  {
    if ( ch1_read == false ) // if radio channel 1 (pin 3) is high and its read value was low last time this interrupt was called (rising edge occurred)
    {
      ch1_read    = true; // mark that the value is read high this time
      timer1count = cnt;  // record the timer count value - to be used later on the falling edge.
    }
  }
  else if ( ch1_read == true ) // if radio channel 1 (pin 3) is Low and its read value was high last time this interrupt was called (falling edge occurred)
  {
    ch1_read = false;                                    // mark that the value is read low this time
    if ( cnt < timer1count )                             // Timer reset during the read of this pulse
      timer1diff = ( cnt + TIMERMAXCNT - timer1count );  // track the timer difference between rising and falling edges
    else
      timer1diff = ( cnt - timer1count ); // track the timer difference between rising and falling edges
  }
  return;
} // end of Timer_0_1_ISR()

/*
 * Class:		ArduRadio
 * Function:	Timer_2_3_ISR()
 * Scope:		Public
 * Arguments:	None
 * Description:	Interrupt Service routine for timers 2 and 3
 *                              This function is used to read the timer values
 *                              and record the differences so that the channel
 *                              2 and 3 PWM values can be interpreted.
 */
static void Timer_2_3_ISR ( void ) {
  uint16_t cnt = TCNT1; // read the timer value

#if THROTTLE_PIN == 11 // if pin 11 is selected as the throttle
  if ( PINB & 8 )      // if radio channel 2 (pin 11) is high
  {
#elif THROTTLE_PIN == 13 // if pin 13 is selected as the throttle
  if ( PINB & 32 )       // if radio channel 2 (pin 13) is high
  {
#else
#error "Invalid Pin selection for Throttle Radio Input.  Please select either pin 11 or 13 using the THROTTLE_PIN definition."
#endif


    if ( ch2_read == false ) // if radio channel 2 is high and its read value was low last time this interrupt was called (rising edge occurred)
    {
      ch2_read    = true; // mark that the value is read high this time
      timer2count = cnt;  // record the timer count value - to be used later on the falling edge.
    }
  }
  else if ( ch2_read == true ) // if radio channel 2 is low and its read value was high last time this interrupt was called (falling edge occurred)
  {
    ch2_read = false;                                    // mark that the value is read low this time
    if ( cnt < timer2count )                             // Timer reset during the read of this pulse
      timer2diff = ( cnt + TIMERMAXCNT - timer2count );  // track the timer difference between rising and falling edges
    else
      timer2diff = ( cnt - timer2count ); // track the timer difference between rising and falling edges
  }

#if THROTTLE_PIN == 11 // if pin 11 is selected as the throttle
  if ( PINB & 32 )     // if radio channel 3 (pin 13) is high
  {
#elif THROTTLE_PIN == 13 // if pin 13 is selected as the throttle
  if ( PINB & 8 )        // if radio channel 3 (pin 11) is high
  {
#else
#error "Invalid Pin selection for Throttle Radio Input.  Please select either pin 11 or 13 using the THROTTLE_PIN definition."
#endif
    if ( ch3_read == false ) // if radio channel 3 is high and its read value was low last time this interrupt was called (rising edge occurred)
    {
      ch3_read    = true; // mark that the value is read high this time
      timer3count = cnt;  // record the timer count value - to be used later on the falling edge.
    }
  }
  else if ( ch3_read == true ) // if radio channel 3 is low and its read value was high last time this interrupt was called (falling edge occurred)
  {
    ch3_read = false;                                    // mark that the value is read low this time
    if ( cnt < timer3count )                             // Timer reset during the read of this pulse
      timer3diff = ( cnt + TIMERMAXCNT - timer3count );  // track the timer difference between rising and falling edges
    else
      timer3diff = ( cnt - timer3count ); // track the timer difference between rising and falling edges
  }
  return;
} // end of Timer_2_3_ISR()

// Global Functions


/*
 * Class:   NA
 * Function:  ISR(PCINT2_vect)
 * Scope:   Global
 * Arguments: None
 * Description: Calls the interrupt service routine for timer 0&1.
 */
ISR ( PCINT2_vect )
{
  Timer_0_1_ISR ( ); // call the timer 0&1 interrupt service routine

  return;
} // end of ISR(PCINT2_vect)

/*
 * Class:   NA
 * Function:  ISR(PCINT0_vect)
 * Scope:   Global
 * Arguments: None
 * Description: Calls the interrupt service routine for timer 2&3.
 */
ISR ( PCINT0_vect )
{
  Timer_2_3_ISR ( ); // call the timer 2&3 interrupt service routine
  return;
} // end of ISR(PCINT2_vect)

}
