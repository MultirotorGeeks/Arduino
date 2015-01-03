/*
 * ArduRadios.h
 *
 *  Created on: Jan 18, 2014
 *      Author: Rancuret
 */

#ifndef ARDURADIOS_H_
#define ARDURADIOS_H_

// Include Files
#include "Arduino.h"

// Definitions
#define CH_ROLL           0     // Index of Roll radio channel
#define CH_PITCH          1     // Index of Pitch radio channel
#define CH_THROTTLE       2     // Index of Throttle radio channel
#define CH_RUDDER         3     // Index of Rudder radio channel
#define ROLL_INITIAL      1500  // Initial (default) value of roll channel.  On ground powerup, the radio stick position offset is calculated to achieve this value.
#define PITCH_INITIAL     1500  // Initial (default) value of roll channel.  On ground powerup, the radio stick position offset is calculated to achieve this value.
#define THROTTLE_INITIAL  1000  // Initial (default) value of roll channel.  On ground powerup, the radio stick position offset is calculated to achieve this value.
#define RUDDER_INITIAL    1500  // Initial (default) value of roll channel.  On ground powerup, the radio stick position offset is calculated to achieve this value.
#define THROTTLE_PIN      13    // pin 13, or pin 11 only.  Tells which pin (D13 or D11) the throttle PWM input is connected to
#define TIMERMAXCNT       40000 // maximum count value of the timer used to measure PWM duty
#define RADIOPWM_MAX      2000  // maximum time value of any radio pwm pulse (microseconds)
#define RADIOPWM_MIN      1000  // minimum time value of any radio pwm pulse (microseconds)
#define GNDINIT_NUMREADS  100   // number of readings to take while calculating average readings during ground initialization
#define GNDINIT_DELAY_MS  10    // number of milliseconds delay inbetween each reading for average calculation during ground initialization
#define RADIO_RTN_SUCCESS 0     // value to return on success when reading radio
#define RADIO_RTN_FILTERR 1     // value to return when filtering error occurred while reading radio

//#define DEBUG_RADIO_ISR   // defining this variable causes debug messages to print for checking the radio interrupt service routine

// ArduRadio Class Definition
/*
 * Class:		ArduRadio
 * Function:	NA
 * Scope:		global
 * Arguments:	NA
 * Description:	This class contains the functions and data necessary
 *                              to read and interpret the incoming radio signals
 *                              in the ArduPilot board.
 */
class ArduRadio
{
private:
uint16_t radio_in [ 4 ];      // Four unsigned integers contain the latest read radio signal values
int16_t  radio_offsets [ 4 ]; // Offset values for each radio channel

boolean  use_filters;         // Logic value which determines whether or not to use any filters
uint8_t  filtcoeffs [ 4 ];    // Filter coefficients for each channel.  This number, which must lie in the range [0,31], determines how much filtering to apply.  The larger the number, the lower the effective filter cutoff frequency.

public:
ArduRadio ( void );                             // Constructor for the ArduRadio class object
uint8_t groundInitNoFilt ( void );              // Ground Initialization function for radios without filters.  The Ground initialization will automatically calculate radio offsets to produce default values.  This sets the radios up with no filtering.
uint8_t groundInitFilt ( uint8_t *filtCoeffs ); // Ground Initialization function for radios with filters.  The Ground initialization will automatically calculate radio offsets to produce default values.  This sets the radios up with specified filter coefficients.
uint8_t airInitNoFilt ( void );                 // Air Initialization function for radios without filters.  The Air initialization will use default (zero) values for offsets.  This sets the radios up with no filtering.
uint8_t airInitFilt ( uint8_t *filtCoeffs );    // Air Initialization function for radios with filters.  TThe Air initialization will use default (zero) values for offsets.  This sets the radios up with specified filter coefficients.
uint8_t read_radio ( uint16_t *radiovals );     // Function which reads the radio values, stores them in the specified array, and returns 0 upon success
};

// global classes
extern ArduRadio ArduRadios; // declaration of the ArduRadios class

#endif /* ARDURADIOS_H_ */
