#pragma once

/* CarT library by CybSe@CAR
 */

// This will load the definition for common Particle variable types
#include "Particle.h"
#include "AWS.h"
#include "Crypt.h"
#include "DOF.h"
#include "CAN.h"
#include "SD.h"

// This is your main class that users will import into their application
class CarT
{
public:
  /**
   * Constructor
   */
  CarT();

  /**
   * Example method
   */
  void begin();

  /**
   * Example method
   */
  void process();
  
private:
  /**
   * Example private method
   */
  void doit();
};
