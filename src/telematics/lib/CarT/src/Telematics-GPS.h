#pragma once

/* CarT library by CybSe@CAR
 */

// This will load the definition for common Particle variable types
#include "Particle.h"
#include "Particle-GPS.h"


// This is your main class that users will import into their application


class Tgps
{
public:
  /**
   * Constructor
   */
   Tgps();

  /**
   * Begin communication
   */
  void begin();

  /*
   * End communication
   */
  void end();

  /**
   * Rertrieve data
   */
  String retrieve();

  /*
   * Retrieve data from serial port
   */
   void onSerialData();


private:

   Gps gps = Gps(&Serial1); // gps object

};
