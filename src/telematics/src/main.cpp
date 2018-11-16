#include "Particle.h"

#include <CarT.h>
//#include <SparkJson.h>


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
    delay(5000);
    //os_mutex_lock(mqtt_mutex);
    RGB.color(255, 0, 0);
    WITH_LOCK(Serial)
    {
        RGB.color(0, 255, 0);
        pinMode(D6,OUTPUT);
        digitalWrite(D6,LOW);
        //sd_storage->begin();
        //dof->getTemp();


        //sd_storage->write(dof->TEMP);
        //sd_storage->write('\n');
        Serial.println();
        secretStuff->generateKey();
        /*
        *   test encryption
        *   using known string hello
        */     
        unsigned char input[128];
        unsigned char output[128];
        unsigned char in[128];
        unsigned char put[128];

        memset(input, 0, 128);
        memset(output, 0, 128);
        memset(in, 0, 128);
        memset(put, 0, 128);

        memcpy(input,"hello",strlen("hello"));

        Serial.print("Encrypted Data: ");
        secretStuff->encryptData(input,output);
        for(int i = 0; i < 16; i++){
            Serial.printf("%d",output[i]);
        }

        Serial.println();

        Serial.print("Decrypted Data: ");
        secretStuff->decryptData(output,put);
        for(int i = 0; i < 16; i++){
            Serial.printf("%c",put[i]);
        }
        Serial.println();
        

    }
    


}   

void loop() {

//process information
#if (MQTT_STATUS||SD_STATUS)
    //print out new informaiton sent from can bus
    if (new_can_flag) os_mutex_lock(can_recv_mutex);      //lock out buffer for reading
    if (new_gps_flag) os_mutex_lock(gps_recv_mutex);      //lock out buffer for reading
    if (new_dof_flag) os_mutex_lock(dof_recv_mutex);      //lock out buffer for reading

    byte message_id;
    message_id = message_id || (new_can_flag << 3);
    message_id = message_id || (new_can_flag << 2);
    message_id = message_id || (new_can_flag << 1);

    //sd write of can messages
    for(int i =0; i < can_frames_in_buffer; i++)
    {
        for(int j = 0; j <8; j++)
        {
            //write in one frame at a time
            sd_storage->write((char)can_recv_buffer[i][j]);  
        }       
    }
    sd_storage->write('\n');   

    new_can_flag = false;   //only change when buffer is locked
    os_mutex_unlock(can_recv_mutex);
    os_mutex_unlock(gps_recv_mutex);
    os_mutex_unlock(dof_recv_mutex);
#endif

    delay(20);
}


