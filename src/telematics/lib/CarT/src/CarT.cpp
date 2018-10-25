/* CarT library by CybSe@CAR
 */

#include "CarT.h"

extern AWS thing();
extern Crypt secretStuff();
extern DOF cartChar();

/**
 * Constructor.
 */
CarT::CarT()
{
  // be sure not to call anything that requires hardware be initialized here, put those in begin()
}

/**
 * Example method.
 */
void CarT::begin()
{
    // initialize hardware
    Serial.println("called begin");
}

/**
 * Example method.
 */
void CarT::process()
{
    // do something useful
    Serial.println("called process");
    doit();
}

/**
* Example private method
*/
void CarT::doit()
{
    Serial.println("called doit");
}
