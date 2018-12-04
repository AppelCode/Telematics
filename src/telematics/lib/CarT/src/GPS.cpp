#include "GPS.h"

GPS::GPS(){
	//constructor
}


void GPS::begin()
{
  gps.begin(9600);
  gps.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  delay(1000);
  gps.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  pinMode(D6, OUTPUT);
  digitalWrite(D6, LOW);

  LAT=0;
  LON = 0;
}

bool GPS::flag(){
  bool condition;
  (gps.fix) ? condition = TRUE : condition = FALSE;
  return condition; 
}

void GPS::getCoord()
{
    while (GPSSerial.available()){
        char c = gps.read();
        if (gps.newNMEAreceived()){
            gps.parse(gps.lastNMEA());
        }
    }
	LAT = gps.latitudeDegrees;
	LON = gps.longitudeDegrees; 	      
	
}

void GPS::time(){
    while (GPSSerial.available()){
        char c = gps.read();
        if (gps.newNMEAreceived()){
            gps.parse(gps.lastNMEA());
            }
    }
    HR = gps.hour;
    MIN = gps.minute;
    SEC = gps.seconds;
    MIL = gps.milliseconds;
    D = gps.day;
    M = gps.month;
    Y = gps.year;
}