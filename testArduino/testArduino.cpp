/*
 * testArduino.cpp
 *
 *  Created on: Dec 20, 2014
 *      Author: Paul
 */

#include <Arduino.h>
#include "ArduRadios.h"


// end of add your includes here
#ifdef __cplusplus
extern "C" {
#endif
void loop ( );
void setup ( );
#ifdef __cplusplus
} // extern "C"
#endif

#define LOOPTIME_MICROSEC 100000
#define RADIOFILTCOEFF_NA 20
#define SERIALBAUD        38400
#define STRBUFFMAX        256

void loop ( void )
{
  uint16_t             radiovals [ 4 ] = { 1500, 1500, 1000, 1500 }; // values read from the radio
  char                 str [ STRBUFFMAX ];                           // string to use when printing

  static unsigned long lastTime = 0;          // time of last loop iteration
  unsigned long        thisTime = micros ( ); // time of this loop iteration

  if ( ( thisTime - lastTime ) >= LOOPTIME_MICROSEC ) // if enough time has elapsed
  {
    /* Print the time stamp */
      snprintf ( str, STRBUFFMAX, "Time: %lu.%02lu s",
      ( thisTime / 1000000 ),
      ( ( thisTime % 1000000 ) / 10000 ) );
    Serial.print ( str );

    if ( ArduRadios.read_radio ( radiovals ) == RADIO_RTN_SUCCESS ) // read radios
    {
      /* Print Radio Values when a successfully read */
      snprintf ( str, STRBUFFMAX, ",   Roll: %4u   Pitch: %4u   Throttle: %4u    Yaw: %4u",
        radiovals [ CH_ROLL ],
        radiovals [ CH_PITCH ],
        radiovals [ CH_THROTTLE ],
        radiovals [ CH_RUDDER ] );
      Serial.println ( str );
    }
    else // problem occurred reading radios
    {
      /* Print Error Message when radio read failed */
      Serial.println ( ",  Error Reading Radio Values!" );
    }

    lastTime = thisTime;

  }


  return;
}

void setup ( void )
{
  uint8_t filterCoeffs [ 4 ] = { RADIOFILTCOEFF_NA, RADIOFILTCOEFF_NA,
                                 RADIOFILTCOEFF_NA, RADIOFILTCOEFF_NA }; // initialize filter coefficients

  Serial.begin ( SERIALBAUD );                // begin serial communications
  ArduRadios.groundInitFilt ( filterCoeffs ); // initialize radio inputs

  return;
}

// Added this comment using eclipse in my main workspace.
// Added this comment in a separate, test copy of the git repository.

// Added this comment in a separate, test copy of the repo, in a newly created branch called otherTest
// Added this comment using eclipse in my main workspace, in a new separate branch called paulTest
