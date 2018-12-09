#include "Particle.h"
#include <ArduinoJson.h>
#include <CarT.h>
#include "SdFat.h"
//#include <SparkJson.h>


SYSTEM_THREAD(ENABLED);     //setup threading
SYSTEM_MODE(MANUAL);        //system mode to disable auto cell connect        
STARTUP(startup_function());//setup stratup function and block everything untill finished

//holds encryption key
unsigned char key[32];
#define ONE_DAY_MILLIS (24 * 60 * 60 * 1000)
unsigned long lastSync = millis();

unsigned char internal_settings = DEFAULT_INTERNAL;
unsigned char can_settings = DEFAULT_CAN;

int timer = 0;
int records = 0;
int can_records = 0;
int internal_records = 0;

//SdFat sd_test;
//File myfile;

void setup() {
    
    Serial.begin(9600); //start Serial output
    delay(5000);        //wait for user, needs to be updated

    //setup 
    dof->begin();       //turn on dof board 
       
    //turn on gps
    pinMode(D6, OUTPUT);
    digitalWrite(D6,LOW);
    gps.begin(9600);
    gps.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    delay(500);
    gps.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
    delay(500);

    sd_storage->begin();    

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
    Serial.println((char*)output); 

    Serial.println();

    Serial.print("Decrypted Data: ");
    secretStuff->decryptData(output,put);
    Serial.println((char*)put); 
  
    timer = millis();
}
void loop() {


    if (millis() - timer > 100){
        timer = millis();

        internal_function(internal_records,internal_settings);     //record gps and dof data

        stn->GetRPM();
        delay(50);
        stn->receive(temp_can_buffer[can_records],4);
        if(temp_can_buffer[can_records][0] != 0){
          can_records++;  
        }
           
        delay(50);

        stn->GetSpeed();
        delay(50);
        stn->receive(temp_can_buffer[can_records],4);
        if(temp_can_buffer[can_records][0] != 0){
          can_records++;  
        }

        if(can_records>internal_records){
            records = can_records;
        } else {
            records = internal_records;
        }

        Serial.print(records);
    #if MQTT_STATUS
        if(os_mutex_trylock(mqtt_mutex)){
        
            //initialize the json parser
            DynamicJsonBuffer jsonBuffer;
            unsigned char message_id = 0;

            for(int i = 0; i < records; i++){
                       
                //meesage type
                message_id = 0;
                if(internal_records > i) 
                {
                    new_gps_flag = true;
                    //new_dof_flag = true;
                }
                if(can_records > i )
                {
                    new_can_flag = true;
                }
                message_id = message_id | (new_can_flag << 3);
                //message_id = message_id | (new_dof_flag << 2);
                message_id = message_id | (new_gps_flag << 1);

                
                //create object for json parsing
                JsonObject& root = jsonBuffer.createObject();
                if(new_can_flag){
                    JsonArray& CAN_data = root.createNestedArray("CAN_data");
                    for(int j =0; j < 16; j++){                     //loop through current record and store
                        CAN_data.add((char)temp_can_buffer[i][j]);  //add data to json data structure
                    } 
                }
                if(new_gps_flag){
                    JsonArray& GPS_data = root.createNestedArray("GPS_data");
                    for(int j =0; j < 2; j++){                   //loop through current record and store
                        GPS_data.add(temp_gps_buffer[i][j]);     //add data to json data structure
                    }
                }
                if(new_dof_flag){
                    JsonArray& DOF_data = root.createNestedArray("DOF_data");
                    for(int j =0; j < 9; j++){                  //loop through current record and store
                        DOF_data.add(temp_dof_buffer[i][j]);    //add data to json data structure
                    }
                }

                root["messageid"] = message_id;                 //store message id             
              

                int buflen= root.measureLength();
                mqtt_send_buffer = (char*)realloc(mqtt_send_buffer,buflen+1);   //adjsut mqtt buffer size
                root.printTo(mqtt_send_buffer,buflen+1);                        //create send char array  

                if(message_id != 0){
                    root.prettyPrintTo(Serial);                     //create send char array
                    sd_storage->write(mqtt_send_buffer);
                    sd_storage->write("\n");
                    awsiot->publish("cart/1",mqtt_send_buffer);     //aws send new buffer  
                }
                
                jsonBuffer.clear();
                
                new_can_flag = false;
                new_dof_flag = false;
                new_gps_flag = false;
            }      
        
            os_mutex_unlock(mqtt_mutex);
        }
    #endif

    records = 0;
    can_records = 0;
    internal_records = 0;   

        
    }
}


