#include "Particle.h"

#include <CarT.h>


SYSTEM_THREAD(ENABLED);     //setup threading
SYSTEM_MODE(MANUAL);        //system mode to disable auto cell connect        
STARTUP(startup_function());//setup stratup function and block everything untill finished

//holds encryption key
unsigned char key[32];

#define ONE_DAY_MILLIS (24 * 60 * 60 * 1000)
unsigned long lastSync = millis();
int counter = 0;
void setup() {


    Serial.begin(9600);
    while(!Serial);
    delay(5000);

    secretStuff->generateKey(key);
    Serial.print("key: ");
    Serial.println((int)key);

    Serial.println(System.freeMemory());
    unsigned char input[128] = "hello world";
    unsigned char output[128];

    for(int i = 11; i < 128; i++){
        input[i] = '0';
    }

    secretStuff->encryptData(input,output);
    for(int i = 0; i < 11; i++){
        Serial.printf("%c",output[i]);
    }
    secretStuff->decryptData(output,input);
    Serial.println();
    for(int i = 0; i < 11; i++){
        Serial.printf("%c",input[i]);
    }

    /*
    //test dof
    dof.getTemp();          //aquire dof temp
    float temp = dof.TEMP;  //set to temp


    //test sd storage
    if (sd_storage.write<float>(temp)){
        Serial.println("bottom of code");
    } else {
        Serial.println("did not write to sd!");
    }
    */

    


}   

void loop() {

    //curently does nothing
    delay(200);
}

