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

int size;

mbedtls_ctr_drbg_context ctr_drbg;
mbedtls_entropy_context entropy;
unsigned char rn[1];

 

//create Gps object
Adafruit_GPS gps = Adafruit_GPS(&Serial1);
int timer;

int buf[64];


void setup() {
    
    Serial.begin(9600); //start Serial output
    delay(5000);        //wait for user, needs to be updated
    
    mbedtls_entropy_init( &entropy );
    mbedtls_ctr_drbg_init( &ctr_drbg );

    mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy, \
    (unsigned char *) "hello", strlen( "hello" ) );



    /*
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

    timer = millis();
    */
}
void loop() {


    stn->begin();
    stn->getRPM();
    int num = stn->receive(buf,0);

    for(int i = 0; i < num; i++){
        Serial.printf("%c", buf[i]);
    }
    /*
    os_mutex_lock(dof_recv_mutex);    
    os_mutex_lock(gps_recv_mutex);
    os_mutex_lock(can_recv_mutex);   

    //meesage type
    unsigned char message_id = 0;;
    message_id = message_id | (new_can_flag << 3);
    message_id = message_id | (new_dof_flag << 2);
    message_id = message_id | (new_gps_flag << 1);

        //initialize the json parser
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    JsonArray& CAN_data = root.createNestedArray("CAN_data");
    JsonArray& GPS_data = root.createNestedArray("GPS_data");
    JsonArray& DOF_data = root.createNestedArray("DOF_data");

    root["messageid"] = message_id;

    if(new_dof_flag){
        for(int i =0; i < dof_frames_in_buffer; i++)
        {
            JsonArray& temp = DOF_data.createNestedArray(); 
            for(int j =0; j < 9; j++){
                temp.add(dof_recv_buffer[i][j]);  
            }
        }
        new_dof_flag = false; 
    }

    if(new_can_flag){
        for(int i =0; i < can_frames_in_buffer; i++)
        {
            JsonArray& temp = CAN_data.createNestedArray(); 
            for(int j =0; j < 9; j++){
                temp.add(can_recv_buffer[i][j]);  
            }     
        }
        new_can_flag = false; 
    }

    if(new_gps_flag){
        
        for(int i =0; i < gps_frames_in_buffer; i++)
        {
            JsonArray& temp = GPS_data.createNestedArray(); 
            for(int j =0; j < 9; j++){
                temp.add(gps_recv_buffer[i][j]);  
            };       
        }
        new_gps_flag = false;  
    }
   
    if(message_id != 0){
        Serial.println("Global Positioning System");
        Serial.println("======================================================");
        Serial.print("Latitude: "); Serial.println(gps.latitudeDegrees);
        Serial.print("Longitude: "); Serial.println(gps.longitudeDegrees);
        Serial.println("");
        
        root.prettyPrintTo(Serial);
    }
    
#if MQTT_STATUS
    os_mutex_lock(mqtt_mutex);
    int buflen= root.measurePrettyLength();
    mqtt_send_buffer = (char*)realloc(mqtt_send_buffer,buflen+1);   //adjsut mqtt buffer size
    root.prettyPrintTo(mqtt_send_buffer,buflen+1);                 //create send char array  
    //awsiot->publish("cart/2",mqtt_send_buffer);                     //aws send new buffer
    os_mutex_unlock(mqtt_mutex);
#endif

    jsonBuffer.clear();

    os_mutex_unlock(dof_recv_mutex);
    os_mutex_unlock(gps_recv_mutex);
    os_mutex_unlock(can_recv_mutex);
    */

}

