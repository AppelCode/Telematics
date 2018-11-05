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
//STARTUP(startupFunction());
bool startup=true;

//MQTT server intalization
void callback(char* topic, byte* payload, unsigned int length);
extern AWS* awsiot = new AWS("a3mb0mz6legbs8.iot.us-east-2.amazonaws.com", 8883, callback);
Thread thread("server_thread", server_thread, OS_THREAD_PRIORITY_DEFAULT,10*1024);

//come from CART currently testing
//components of Telematics device
CAN stn;
DOF dof;
Tgps gps;
SD sd_storage;
Gps _gps = Gps(&Serial1);
Gga* gga;
Crypt secretStuff;

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
   
    sd_storage.begin(); //setup SD   
    _gps.begin(9600);   //setup GPS   
    dof.begin;          //DOF begin   
    stn.begin();        //setup CAN

    //CAN
    int buffer[64];
    int size=0;
    stn.receive(buffer,size);

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

    //used for testing, allows tera term to set up connection
    Serial.begin(9600);
    while(!Serial);

    //unlock recv buffer for use
    os_mutex_unlock(recv_mutex);
    //startup complete                  
    startup=true;

}

// recieve message handler for server_thread
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

//AWS server thread that poles for MQTT requests on different subscribed nodes
void server_thread(void) {
    
    while(startup == false);            //wait for startup to finish
    awsiot->connect("sparkclient");     //setup AWS connection
        if (awsiot->isConnected()) {
            awsiot->publish("outTopic/message", "hello world");
            awsiot->subscribe("inTopic/message");
        }

    RGB.color(0, 255, 0);                               //valid aws connection
    delay(1000);
	while(true) {       
        os_mutex_lock(recv_mutex);       
        if (awsiot->isConnected()) {
            
            awsiot->loop();
        }
        os_mutex_unlock(recv_mutex);
        os_thread_delay_until(&lastThreadTime, 10);
	} 
}

