// Example usage for Cart library by CybSe@CAR.

#include "Cart.h"

// Initialize objects from the lib
Cart cart;

void setup() {
    // Call functions on initialized library objects that require hardware
    cart.begin();
}

void loop() {
    // Use the library's initialized objects and functions
    cart.process();
}
