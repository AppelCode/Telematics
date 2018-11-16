
#include "SdFat.h"
#include "Particle.h"
#include "SD.h"

SD::SD()
{
    //initialize variables
}

bool SD::begin()
{

    /* Initialize SdFat or print a detailed error message and halt
    *  Use half speed like the native library.
    *  Change to SPI_FULL_SPEED for more performance.
    */
    if (!sd.begin(chipSelect, SPI_HALF_SPEED)) {
      return false;
    }
    return true;
}


void SD::read()
    {
        if (!myFile.open("test.txt", O_READ)) {
        }
        // read from the file until there's nothing else in it:
        int data;
        while ((data = myFile.read()) >= 0) {
        }
        // close the file:
        myFile.close();

    }