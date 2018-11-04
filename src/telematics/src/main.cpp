#include "Particle.h"
#include "cellular_hal.h"
#include <CarT.h>

//setup threading
SYSTEM_THREAD(ENABLED);
os_mutex_t recv_mutex;
char* recv_buffer;
system_tick_t lastThreadTime = 0;

//system mode to disable auto cell connect
SYSTEM_MODE(MANUAL);

//start up and thread functions
void startupFunction();              
void server_thread(void);         

//setup stratup function and block everything untill finished
STARTUP(startupFunction());
bool startup=false;

//mutex to be used to block thread untill needed

Thread thread("server_thread", server_thread);

//#include <CarT.h>
void callback(char* topic, byte* payload, unsigned int length);

/**
 * if want to use IP address,
 * byte server[] = { XXX,XXX,XXX,XXX };
 * MQTT client(server, 1883, callback);
 * want to use domain name,
 * MQTT client("www.sample.com", 1883, callback);
 **/

AWS* awsiot = new AWS("a3mb0mz6legbs8.iot.us-east-2.amazonaws.com", 8883, callback);  
Crypt secretStuff;                        

//come from CART currently testing
CAN stn;
DOF dof;
SD sd_storage;

//holds encryption key
unsigned char key[32];

// recieve message
//used for handling all subscription messages
//will be using for updating recv buffer
void callback(char* topic, byte* payload, unsigned int length) {
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = NULL;
    String message(p);

    if (message.equals("RED"))
        RGB.color(255, 0, 0);
    else if (message.equals("GREEN"))
        RGB.color(0, 255, 0);
    else if (message.equals("BLUE"))
        RGB.color(0, 0, 255);
    else
        RGB.color(255, 255, 255);
    delay(1000);
}


#define ONE_DAY_MILLIS (24 * 60 * 60 * 1000)
unsigned long lastSync = millis();
int counter = 0;
void setup() {

    //block untill startup is complete
    while(startup == false);
    WITH_LOCK(Serial){

        //setup SD
        sd_storage.begin();

        //setup 9DOF
        dof.begin();

        //setup CAN
        //stn.begin();

        //used for testing, allows tera term to set up connection
        delay(5000);
        Serial.println(System.freeMemory());

        //rts resync (not used)
        /*
        resync time everyday
        if (millis() - lastSync > ONE_DAY_MILLIS) {
            Particle.syncTime();
            lastSync = millis();
        }
        */

        //dof info
        dof.getTemp();
        float temp = dof.TEMP;
        Serial.print("dof temp: ");
        Serial.println(temp);

        //test sd storage
        if (sd_storage.write<float>(temp)){
            Serial.println("wrote to sd!");
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


}   

void loop() {

    //curently does nothing
    delay(200);
}



//use for thread setup
//use for connection setup
void startupFunction() {

     // Create the mutex and lock it
	os_mutex_create(&recv_mutex);
	os_mutex_lock(recv_mutex);

    //setup Cellular
    cellular_credentials_set("wireless.twilio.com", "", "", NULL);  //setup to work twilio sim
    Cellular.on();                                                  //turn on celluar modem
    Cellular.connect();                                             //connect using twillio
    while(!Cellular.ready());                                       //wait until connected
    RGB.control(true);                                              //set RGB to control
    RGB.color(255, 0, 0);                                           //RED shows cellular connection is valid

    //startup complete                  
    startup=true;

}

//AWS server thread that poles for MQTT requests on different subscribed nodes
void server_thread(void) {
    //setup AWS connection
    while(startup == false);

    //RGB.color(0, 255, 0);                               //valid aws connection
    
	while(true) {
        os_mutex_lock(recv_mutex);
        /*
        if (awsiot.isConnected()) {
            awsiot.loop();
        }
        */
        os_thread_delay_until(&lastThreadTime, 10);
	} 
}

