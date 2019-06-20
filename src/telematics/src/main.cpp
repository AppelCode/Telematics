#include "Particle.h"
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
int can_records = 0;
int internal_records = 0;

//1KB of data can be sent
char jsonMessage[1000];

const int num_pids = 10;
char* pid_list[num_pids] = {PID_FUEL_TANK, PID_ENGINE_LOAD,
		PID_THROTTLE_POSITION,PID_SPEED, PID_RPM, PID_TIMING_ADVANCE,
		PID_ENGINE_FUEL_RATE,PID_ENGINE_REFERENCE_TORQUE,PID_ACTUAL_GEAR};


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

    //start can communication
    stn->begin();
    Serial5.println("ATSH7DF");

    //sd_storage->begin();

    /*
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
    */
  
    timer = millis();
}
void loop() {

	//run every 2s
    if (millis() - timer > 2000){
        timer = millis();

        //internal_function(internal_records,internal_settings);
        CAN_function(pid_list,num_pids,can_records);

    #if MQTT_STATUS
        if(os_mutex_trylock(mqtt_mutex)){

        	memset(jsonMessage,'\0',sizeof(jsonMessage));

        	//set the message type
            uint8_t message_id = 0;
			message_id = 0;
			if(internal_records > 0)
			{
				new_gps_flag = true;
				new_dof_flag = true;
			}
			if(can_records > 0 )
			{
				new_can_flag = true;
			}
			message_id = message_id | (new_can_flag << 3);
			message_id = message_id | (new_dof_flag << 2);
			message_id = message_id | (new_gps_flag << 1);
			char message_num[3];
			snprintf(message_num,3,"%d",message_id);

			//start the creation of the json object
			strcpy(jsonMessage, "{\"messageid\": ");		//enter messageid object
			strcat(jsonMessage, message_num);		//enter messageid object value
			strcat(jsonMessage, ",");					//finish this object entry

			//json can data structure
			if(new_can_flag){
				strcat(jsonMessage, "{\"CAN_data\":[");

				for(int i = 0; i< can_records; i++){
					strcat(jsonMessage, "{\"frame\": [");
					char temp[16];

					for(int j =0; j < strlen((char *)temp_can_buffer[i]); j++){  	//loop through current record and store
						snprintf(temp,16,"%d", temp_can_buffer[i][j]);
						strcat(jsonMessage,temp);									//add data to json data structure

						if (j != strlen((char *)temp_can_buffer[i])){
							strcat(jsonMessage, ",");
						}

					}
					strcat(jsonMessage, "]");
				}
				strcat(jsonMessage, "]");
			}
			/*
			if(new_gps_flag){
				JsonArray& GPS_data = root.createNestedArray("GPS_data");
				for(int j =0; j < 2; j++){                   //loop through current record and store
					//GPS_data.add(temp_gps_buffer[i][j]);     //add data to json data structure
				}
			}
			if(new_dof_flag){
				JsonArray& DOF_data = root.createNestedArray("DOF_data");
				for(int j =0; j < 9; j++){                  //loop through current record and store
					//DOF_data.add(temp_dof_buffer[i][j]);    //add data to json data structure
				}
			}
			*/

			//end json object
			strcat(jsonMessage, "}");

			if(message_id != 0){
				//sd_storage->write(mqtt_send_buffer);
				//sd_storage->write("\n");
				awsiot->publish("cart/1",jsonMessage);    		//aws send new buffer
			}

			new_can_flag = false;
			new_dof_flag = false;
			new_gps_flag = false;
        
            os_mutex_unlock(mqtt_mutex);
        }
    #endif

    can_records = 0;
    internal_records = 0;   

        
    }
}


