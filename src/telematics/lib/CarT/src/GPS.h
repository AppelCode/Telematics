#ifndef GPS_H
#define GPS_H
#include "Adafruit_GPS.h"
#define GPSSerial Serial1

// This is your main class that users will import into their application


class GPS
{
public:

  GPS();

  void begin();
  void getCoord(); 
  bool flag(); 
  void time();
  
  float LAT, LON;
  uint8_t HR, MIN, SEC, Y, M, D;
  uint16_t MIL;
  

private:

  Adafruit_GPS gps = Adafruit_GPS(&GPSSerial);

};
#endif
