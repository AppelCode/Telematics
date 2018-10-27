// This #include statement was automatically added by the Particle IDE.
#include "Particle.h"
#include "Telematics-GPS.h"
#include <MQTT-TLS.h>
#include <CarT.h>


//setup threading
SYSTEM_THREAD(ENABLED);
SYSTEM_MODE(MANUAL);

// Power management on Electron
PMIC _pmic;
Tgps gps;

void onSerialData();

// ***
// *** Create a timer that fires every 1 ms to capture
// *** incoming serial port data from the GPS.
// ***
Timer _timer = Timer(1, onSerialData);


void setup() {
    Cellular.off(); // switch of Cellular
    _pmic.disableCharging(); // switch off battery

    RGB.control(true);
    RGB.color(0, 5, 0); // make LED green
    // ***
    // *** Initialize the USB Serial for debugging.
    // ***
    Serial.begin();
    Serial.println("Initializing...");

    // ***
    // *** Initialize the GPS.
    // ***
    gps.begin();

    // ***
    // *** Start the GPS timer.
    // ***
    _timer.start();

}


void loop() {

    // send gps data
    Serial.println("nl");
    Serial.println(gps.retrieve());
    delay(1000);
}


// GPS read timer

void onSerialData()
{
  gps.onSerialData();
}
