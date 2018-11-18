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

<<<<<<< HEAD

//create json buffer for parsing data 
const size_t bufferSize = 3*JSON_ARRAY_SIZE(2) + JSON_OBJECT_SIZE(7);
DynamicJsonBuffer jsonBuffer(bufferSize);
JsonObject& root;
JsonArray& CAN_data;
JsonArray& GPS_data;
JsonArray& DOF_data;
=======
void jsonMessage() {
  //can't get dynamic size working well
  //const size_t bufferSize = 3*JSON_ARRAY_SIZE(3) + JSON_OBJECT_SIZE(5);
  //DynamicJsonBuffer jsonBuffer(bufferSize);
  
  DynamicJsonBuffer jsonBuffer;
   
  JsonObject& jsonMsg = jsonBuffer.createObject();
  
  jsonMsg["time"] = millis();
  jsonMsg["imu"] = "9DOF";


  JsonArray& gyro = jsonMsg.createNestedArray("gyro");
  dof->getGyr();
  gyro.add(dof->GX);
  gyro.add(dof->GY);
  gyro.add(dof->GZ);

  JsonArray& accl = jsonMsg.createNestedArray("accl");
  dof->getAcc();
  accl.add(dof->AX);
  accl.add(dof->AY);
  accl.add(dof->AZ);

  JsonArray& magn = jsonMsg.createNestedArray("magn");
  dof->getMag();
  magn.add(dof->MX);
  magn.add(dof->MY);
  magn.add(dof->MZ);
  
  jsonMsg["gps"] = "GPS";
  JsonArray& lat = jsonMsg.createNestedArray("lat");
  //gps->getCoord();
  //lat.add("this dont work");
  //JsonArray& lon = jsonMsg.createNestedArray("lon");
  //lon.add("this dont work");


  //Serial.println();
  //jsonMsg.prettyPrintTo(Serial);
  //Serial.println();
  
  //this doesn't work
  //jsonMsg.prettyPrintTo(output);
  //sd_storage->begin();
  //sd_storage->write(output);
  
  //send to message to aws
  //not implemeted
}
>>>>>>> 36e6e6141c4b72cffbb0c2f19c477397d4666b84

void setup() {
    
    Serial.begin(9600); //start Serial output
    delay(5000);        //wait for user, needs to be updated

    //initialize the json parser
    JsonObject& root = jsonBuffer.createObject();
    JsonArray& CAN_data = root.createNestedArray("CAN_data");
    JsonArray& GPS_data = root.createNestedArray("GPS_data");
    JsonArray& DOF_data = root.createNestedArray("DOF_data");

    WITH_LOCK(Serial)
    {

        secretStuff->generateKey();

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
///#if (MQTT_STATUS||SD_STATUS)

    os_mutex_lock(dof_recv_mutex); 
    os_mutex_lock(gps_recv_mutex); 
    os_mutex_lock(can_recv_mutex);   
    //meesage type
    byte message_id;
    message_id = message_id || (new_can_flag << 3);
    message_id = message_id || (new_can_flag << 2);
    message_id = message_id || (new_can_flag << 1);

    root["messageid"] = message_id;
    root["CAN_frames"] = can_frames_in_buffer;
    root["DOF_frames"] = dof_frames_in_buffer;
    root["GPS_frames"] = gps_frames_in_buffer;
    if(new_dof_flag){
    
        for(int i =0; i < can_frames_in_buffer; i++)
        {

            //write each array of records to json structure
            //dof_recv_buffer[i][j]);      
        }
        Serial.println();  

        new_can_flag = false;   //only change when buffer is locked
    }

    if(new_can_flag){
    
        for(int i =0; i < gps_frames_in_buffer; i++)
        {
            //write each array of records to json structure
        }
        Serial.println();  

        new_gps_flag = false;   //only change when buffer is locked
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


