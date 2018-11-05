#include "Particle.h"
#include "cellular_hal.h"
#include <CarT.h>


SYSTEM_THREAD(ENABLED);//setup threading
SYSTEM_MODE(MANUAL);//system mode to disable auto cell connect        
STARTUP(startup_function());//setup stratup function and block everything untill finished

//holds encryption key
unsigned char key[32];

#define ONE_DAY_MILLIS (24 * 60 * 60 * 1000)
unsigned long lastSync = millis();
int counter = 0;
void setup() {

    //rts resync (not used)
    /*
    resync time everyday
    if (millis() - lastSync > ONE_DAY_MILLIS) {
        Particle.syncTime();
        lastSync = millis();
    }
    */

    //block untill startup is complete
    while(startup == false);

    //test dof
    dof.getTemp();          //aquire dof temp
    float temp = dof.TEMP;  //set to temp

    //test gps
    //wait for fix
     _gps.onSerialData();
    *gga = Gga(_gps);   //setup GPS parser

    //test sd storage
    if (sd_storage.write<float>(temp)){
        Serial.println("bottom of code");
    } else {
        Serial.println("did not write to sd!");
    }
        
    Serial.println(System.freeMemory());
    // publish/subscribe
    awsiot->connect("sparkclient");
    if (awsiot->isConnected()) {
        Serial.println("client connected");
        awsiot->publish("outTopic/message", "hello world");
        awsiot->subscribe("inTopic/message");
    }
    Serial.println(System.freeMemory());

    //key genrateion and test
    secretStuff.generateKey(key);
    Serial.print("key: ");
    Serial.println((int)key);



}   

void loop() {

    //curently does nothing
    delay(200);
}

