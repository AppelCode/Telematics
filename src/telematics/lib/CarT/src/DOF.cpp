#include "DOF.h"
#include "SparkFunLSM9DS1.h"
#include "math.h"


//////////////////////////
// LSM9DS1 Library Init //
//////////////////////////
// Use the LSM9DS1 class to create an object. [imu] can be
// named anything, we'll refer to that throught the sketch.

///////////////////////
// Example I2C Setup //
///////////////////////
// SDO_XM and SDO_G are both pulled high, so our addresses are:
#define LSM9DS1_M	0x1E // Would be 0x1C if SDO_M is LOW
#define LSM9DS1_AG	0x6B // Would be 0x6A if SDO_AG is LOW

////////////////////////////
// Sketch Output Settings //
////////////////////////////
#define PRINT_CALCULATED
//#define PRINT_RAW
#define PRINT_SPEED 250 // 250 ms between prints

// Earth's magnetic field varies by location. Add or subtract 
// a declination to get a more accurate heading. Calculate 
// your's here:
// http://www.ngdc.noaa.gov/geomag-web/#declination
#define DECLINATION -7.140 // Declination (degrees) in Columbus, OH.
//In future 


DOF::DOF()
{
	setup();
} 


void DOF::setup() 
{
  imu.settings.device.commInterface = IMU_MODE_I2C;
  imu.settings.device.mAddress = LSM9DS1_M;
  imu.settings.device.agAddress = LSM9DS1_AG;
  imu.begin();
}

void DOF::getGyr()
{
  // To read from the gyroscope, you must first call the
  // readGyro() function. When this exits, it'll update the
  // gx, gy, and gz variables with the most current data.
  imu.readGyro();
  // Now we can use the gx, gy, and gz variables as we please.
  // Either print them as raw ADC values, or calculated in DPS.
#ifdef PRINT_CALCULATED
  // If you want to print calculated values, you can use the
  // calcGyro helper function to convert a raw ADC value to
  // DPS. Give the function the value that you want to convert.
  GX=imu.calcGyro(imu.gx);
  GY=imu.calcGyro(imu.gy);
  GZ=imu.calcGyro(imu.gz);
#elif defined PRINT_RAW
  GX=imu.gx;
  GY=imu.gy;
  GZ=imu.gz;
#endif
}


void DOF::getAcc()
{
  // To read from the accelerometer, you must first call the
  // readAccel() function. When this exits, it'll update the
  // ax, ay, and az variables with the most current data.
  imu.readAccel(); 
  // Now we can use the ax, ay, and az variables as we please.
  // Either print them as raw ADC values, or calculated in g's.
#ifdef PRINT_CALCULATED
  // If you want to print calculated values, you can use the
  // calcAccel helper function to convert a raw ADC value to
  // g's. Give the function the value that you want to convert.
  AX=imu.calcAccel(imu.ax);
  AY=imu.calcAccel(imu.ay);
  AZ=imu.calcAccel(imu.az);
#elif defined PRINT_RAW 
  AX=imu.ax;
  AY=imu.ay;
  AZ=imu.az;
#endif

}

void DOF::getMag()
{
  // To read from the magnetometer, you must first call the
  // readMag() function. When this exits, it'll update the
  // mx, my, and mz variables with the most current data.
  imu.readMag(); 
  // Now we can use the mx, my, and mz variables as we please.
  // Either print them as raw ADC values, or calculated in Gauss.
  Serial.print("M: ");
#ifdef PRINT_CALCULATED
  // If you want to print calculated values, you can use the
  // calcMag helper function to convert a raw ADC value to
  // Gauss. Give the function the value that you want to convert.
  MX=imu.calcMag(imu.mx);
  MY=imu.calcMag(imu.my);
  MZ=imu.calcMag(imu.mz);
#elif defined PRINT_RAW
  MX=imu.mx;
  MY=imu.my;
  MZ=imu.mz;
#endif
}

void DOF::getTemp()
{
  // To read from the temperature, you must first call the
  // readTemp() function. When this exits, it'll update the
  // temperature with the most current data.
  imu.readTemp(); 
  // get temp
  TEMP = (imu.temperature/256 +25.0);
}

// Calculate pitch, roll, and heading.
// Pitch/roll calculations take from this app note:
// http://cache.freescale.com/files/sensors/doc/app_note/AN3461.pdf?fpsp=1
// Heading calculations taken from this app note:
// http://www51.honeywell.com/aero/common/documents/myaerospacecatalog-documents/Defense_Brochures-documents/Magnetic__Literature_Application_notes-documents/AN203_Compass_Heading_Using_Magnetometers.pdf
void DOF::getAtt()
{
  imu.readAccel();
  imu.readMag();
  float TEMPax=imu.ax; 
  float TEMPay=imu.ay; 
  float TEMPaz=imu.az; 
  float TEMPmy=-imu.my; 
  float TEMPmx=-imu.mx; 
  
  roll = atan2(TEMPay, TEMPaz);
  pitch = atan2(-TEMPax, sqrt(TEMPay * TEMPay + TEMPaz * TEMPaz));
  if (TEMPmy == 0)
    heading = (TEMPmx < 0) ? 180.0 : 0;
  else
  heading = atan2(TEMPmx, TEMPmy);
    
  heading -= DECLINATION * M_PI / 180;
  
  if (heading > M_PI) heading -= (2 * M_PI);
  else if (heading < -M_PI) heading += (2 * M_PI);
  else if (heading < 0) heading += 2 * M_PI;
  
  // Convert everything from radians to degrees:
  heading *= 180.0 / M_PI;
  pitch *= 180.0 / M_PI;
  roll  *= 180.0 / M_PI;
}
