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

int timer = 0;


void setup() {
    
    Serial.begin(9600); //start Serial output
    delay(5000);        //wait for user, needs to be updated

    //turn on dof board
    dof->begin(); 
       
    //turn on gps
    pinMode(D6, OUTPUT);
    digitalWrite(D6,LOW);
    gps.begin(9600);
    gps.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    delay(500);
    gps.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
    delay(500);

    //start can communication
    stn->begin();
    Serial5.println("ATSH7DF");

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
    Serial.println(System.freeMemory());  

    timer = millis();
}
void loop() {


    if (millis() - timer > 1000){
        timer = millis();

        internal_function();
        CAN_function();
        //meesage type
        unsigned char message_id = 0;
        message_id = message_id | (new_can_flag << 3);
        //message_id = message_id | (new_dof_flag << 2);
        message_id = message_id | (new_gps_flag << 1);
        //initialize the json parser
        DynamicJsonBuffer jsonBuffer;
        JsonObject& root = jsonBuffer.createObject();
        JsonArray& CAN_data = root.createNestedArray("CAN_data");
        JsonArray& GPS_data = root.createNestedArray("GPS_data");
        JsonArray& DOF_data = root.createNestedArray("DOF_data");

        root["messageid"] = message_id;

        for(int j =0; j < 9; j++){
            DOF_data.add(temp_dof_buffer[j]);  
        }


        for(int j =0; j < 16; j++){
            CAN_data.add(temp_can_buffer[j]);  
        }     
        
        for(int j =0; j < 2; j++){
            GPS_data.add(temp_gps_buffer[j]);  
        }  


    #if MQTT_STATUS
        os_mutex_lock(mqtt_mutex);
        int buflen= root.measureLength();
        mqtt_send_buffer = (char*)realloc(mqtt_send_buffer,buflen+1);   //adjsut mqtt buffer size
        root.printTo(mqtt_send_buffer,buflen+1);                 //create send char array  


        if(message_id != 0){
            root.prettyPrintTo(Serial);                 //create send char array
            awsiot->publish("cart/1",mqtt_send_buffer);                     //aws send new buffer  
        }
    
        os_mutex_unlock(mqtt_mutex);
    #endif

        jsonBuffer.clear();

        new_can_flag = false;
        new_dof_flag = false;
        new_gps_flag = false;
    }
}


