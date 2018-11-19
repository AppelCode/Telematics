#ifndef DOF_H
#define DOF_H
#include "SparkFunLSM9DS1.h"

class  DOF {
	
public:
	DOF();
	float GX, GY, GZ; //example 9DOF object.GX;
	float AX, AY, AZ; //same as above
	float MX, MY, MZ; //make sure to use all CAPS for all 9 of these to prevent conflict
	float heading, pitch, roll;
	float TEMP; //temperature
	
	void getAll(); //this now gets all the values
	float *dofARRAY();

	void begin();
	void getGyr();
	void getAcc();
	void getMag();
	void getAtt();
	bool getTemp();
	
	
private:
    
	LSM9DS1 imu; //LSM9DS1 object

};
#endif
