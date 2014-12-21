/*
 * testArduino.cpp
 *
 *  Created on: Dec 20, 2014
 *      Author: Paul
 */

#include <Arduino.h>
#include "ArduRadios.h"


//end of add your includes here
#ifdef __cplusplus
extern "C" {
#endif
void loop();
void setup();
#ifdef __cplusplus
} // extern "C"
#endif

#define LOOPTIME_MICROSEC 100000
#define RADIOFILTCOEFF_NA 20
#define SERIALBAUD 38400

void loop(void)
{
  uint16_t radiovals[4]; // values read from the radio

  static unsigned long lastTime = 0; // time of last loop iteration
  unsigned long thisTime = micros(); // time of this loop iteration

  if ( thisTime - lastTime >= LOOPTIME_MICROSEC ) // if enough time has elapsed
  {
    if ( ArduRadios.read_radio(radiovals) == RADIO_RTN_SUCCESS ) // read radios
    {
      /*
       * Print Radio Values when a successfully read
       */
      Serial.print("Time: ");
      Serial.print( ( thisTime / 1000000 ) , DEC );
      Serial.print(".");
      Serial.print( ( ( thisTime % 1000000 ) / 10000 ) , DEC );
      Serial.print(",  Roll: ");
      Serial.print( radiovals[CH_ROLL] , DEC );
      Serial.print(",  Pitch: ");
      Serial.print( radiovals[CH_PITCH] , DEC );
      Serial.print(",  Throttle: ");
      Serial.print( radiovals[CH_THROTTLE] , DEC );
      Serial.print(",  Rudder: ");
      Serial.println( radiovals[CH_RUDDER] , DEC );
    }
    else // problem occurred reading radios
    {
      /*
       * Print Error Message when radio read failed
       */
      Serial.print("Time: ");
      Serial.print( ( thisTime / 1000000 ) , DEC );
      Serial.print(".");
      Serial.print( ( ( thisTime % 1000000 ) / 10000 ) , DEC );
      Serial.println(",  Error Reading Radio Values!" );
    }

  }

  lastTime = thisTime;

  return;
}

void setup(void)
{
  uint8_t filterCoeffs[4] = {RADIOFILTCOEFF_NA,RADIOFILTCOEFF_NA,
      RADIOFILTCOEFF_NA,RADIOFILTCOEFF_NA}; // initialize filter coefficients

  Serial.begin(SERIALBAUD); // begin serial communications
  ArduRadios.groundInitFilt(filterCoeffs); // initialize radio inputs

  return;
}

// Added this comment in a separate, test copy of the git repository.