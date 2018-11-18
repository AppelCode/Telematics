#include "Particle.h"
#include <ArduinoJson.h>
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
    /*
    StaticJsonBuffer<200> jsonBuffer;

    // create an object
    JsonObject& object1 = jsonBuffer.createObject();
    object1["hello"] = "world";

    // parse a JSON object
    char json[] = "{\"hello\":\"world\"}";
    JsonObject& object2 = jsonBuffer.parseObject(json);

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
        
        //test encryption
        //using known string hello
  
          
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
    */
    


}   

void loop() {


//process information
///#if (MQTT_STATUS||SD_STATUS)

    os_mutex_lock(dof_recv_mutex); 
    os_mutex_lock(gps_recv_mutex); 
    os_mutex_lock(can_recv_mutex);   
    //meesage type
    byte message_id;
    message_id = message_id || (new_can_flag << 3);
    message_id = message_id || (new_can_flag << 2);
    message_id = message_id || (new_can_flag << 1);
    
    if(new_dof_flag){
    
        for(int i =0; i < dof_frames_in_buffer; i++)
        {

            //write each array of records to json structure
            //dof_recv_buffer[i][j]);      
        }
        Serial.println();  

        new_dof_flag = false;   //only change when buffer is locked
    }

    if(new_can_flag){
    
        for(int i =0; i < dof_frames_in_buffer; i++)
        {
            //write each array of records to json structure
        }
        Serial.println();  

        new_dof_flag = false;   //only change when buffer is locked
    }

    if(new_gps_flag){
    
        for(int i =0; i < dof_frames_in_buffer; i++)
        {

            //write each array of records to json structure      
        }
        Serial.println();  

        new_dof_flag = false;   //only change when buffer is locked
    }

    os_mutex_unlock(dof_recv_mutex);
    os_mutex_unlock(gps_recv_mutex);
    os_mutex_unlock(can_recv_mutex);

    os_mutex_lock(mqtt_mutex);

    //copy json char* into send buffer
    new_mqtt_send_flag = true;
    os_mutex_unlock(mqtt_mutex);
    
//#endif
}


