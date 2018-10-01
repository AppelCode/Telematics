// Example usage for CarT library by CybSe@CAR.

#include "CarT.h"

// Initialize objects from the lib
CarT carT;

void setup() {
    // Call functions on initialized library objects that require hardware
    carT.begin();
}

void loop() {
    // Use the library's initialized objects and functions
    carT.process();
}
