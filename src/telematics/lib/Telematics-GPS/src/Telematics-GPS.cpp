/* Telematics-GPS library by CybSe@CAR
 */

 #include "Particle.h"
 #include "Telematics-GPS.h"
 #include "Particle-GPS.h"
// Gps baud rate
static const int BAUDRATE = 9600;

/**
 * Constructor.
 */
Tgps::Tgps()
{
  	// TX and RX pins on the Photon and Electron.
}

/**
 * Example method.
 */
void Tgps::begin()
{
    // begin the gps
    this->gps.begin(BAUDRATE);
    // Request all data be sent
    this->gps.sendCommand(PMTK_SET_NMEA_OUTPUT_ALLDATA);
}


void Tgps::end()
{
    // initialize hardware
    this->gps.end();
}

/**
 * Parse and return the data as a string
 */
String Tgps::retrieve()
{
    String returnValue = "X";
    Gga gga = Gga(gps); // Call GGA parser

    Rmc rmc = Rmc(gps); // Call RMC parser
    rmc.parse();
    gga.parse();

        // Create gps data as JSON format

        returnValue = "{\"\n\t\"gpsData\":{"
            "\"UTC_Time\":0" +  String(gga.utcTime) + ", " +
            "\"posFix\":" +  String((uint8_t)gga.positionFixIndicator) + ", " +
            "\"numSats\":" + String(gga.satellitesUsed) + ", " +
            "\"Lat\":0" + String(gga.latitude) + ", " +
            "\"Long\":0" + String(gga.longitude) + ", " +
            "\"Alt\":" + String(gga.altitude) + ", " +
            "\"Speed\":" + String(rmc.speedOverGround) + ", " +
            "\"Course\":" + String(rmc.courseOverGround) + "}"
            "\n}";

      return returnValue;
}



void Tgps::onSerialData(){
  gps.onSerialData();
}
