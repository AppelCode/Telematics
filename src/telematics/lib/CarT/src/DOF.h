#ifndef DOF_H
#define DOF_H
#include "SparkFunLSM9DS1.h"

//not sure how functional this code is yet but it's a start. Needs to be tested more.
 
class  DOF {
	
public:
	DOF();
	float GX, GY, GZ; //example 9DOF object.GX;
	float AX, AY, AZ; //same as above
	float MX, MY, MZ;
	float heading, pitch, roll;
	float TEMP;
	

	void getGyr();
	void getAcc();
	void getMag();
	void getAtt();
	void getTemp();
	
	
private:
    
	LSM9DS1 imu; //LSM9DS1 object
	void setup(); //has to be called before gyro, accel, mag, temp or attitude retrieval 

};
#endif
