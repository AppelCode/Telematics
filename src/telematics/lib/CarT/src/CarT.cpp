/* CarT library by CybSe@CAR
 */

#include "CarT.h"

system_tick_t lastThreadTime = 0;
bool startup=false;

//initialize buffers
char* mqtt_recv_buffer;  //buffer for mqqt_recv data
char* mqtt_send_buffer;  //buffer for mqtt_send data 
int*  can_recv_buffer;   //buffer for can_recv data
int*  can_send_buffer;   //buffer for can_send data
void* gps_recv_buffer;   //buffer for gps_recv data
void* dof_recv_buffer;   //buffer for dof_recv data

//mutex locks for buffers
os_mutex_t mqtt_recv_mutex;    
os_mutex_t mqtt_send_mutex;   
os_mutex_t can_recv_mutex;    
os_mutex_t can_send_mutex;    
os_mutex_t gps_recv_mutex;     
os_mutex_t dof_recv_mutex;

CAN stn;
DOF dof;
SD sd_storage;
Gga* gga;
Crypt secretStuff;
Gps _gps = Gps(&Serial1);
AWS* awsiot = new AWS("a3mb0mz6legbs8.iot.us-east-2.amazonaws.com", 8883, callback);

//setup threads
Thread server_thread("server_thread", server_thread_function, OS_THREAD_PRIORITY_DEFAULT,10*1024);
Thread CAN_thread("CAN_thread", CAN_thread_function, OS_THREAD_PRIORITY_DEFAULT,10*1024);
Thread internal_thread("Internal_thread", internal_thread_function, OS_THREAD_PRIORITY_DEFAULT,10*1024);

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

//use for thread setup
//use for connection setup
void startup_function() {

     // Create mutexs
	os_mutex_create(&mqtt_recv_mutex);
    os_mutex_create(&mqtt_send_mutex);
    os_mutex_create(&can_recv_mutex);
    os_mutex_create(&can_send_mutex);
    os_mutex_create(&gps_recv_mutex);
    os_mutex_create(&dof_recv_mutex);

    //lock mutex
	os_mutex_lock(mqtt_recv_mutex);
    os_mutex_lock(mqtt_send_mutex);
    os_mutex_lock(can_recv_mutex);
    os_mutex_lock(can_send_mutex);
    os_mutex_lock(gps_recv_mutex);
    os_mutex_lock(dof_recv_mutex);

    //begin communication between components
    sd_storage.begin(); //setup SD   
    _gps.begin(9600);   //setup GPS   
    dof.begin();        //DOF begin   
    stn.begin();        //setup CAN

    //setup Cellular
#if CELLULAR
    cellular_credentials_set("wireless.twilio.com", "", "", NULL);  //setup to work twilio sim
    Cellular.on();                                                  //turn on celluar modem
    Cellular.connect();                                             //connect using twillio
    while(!Cellular.ready());                                       //wait until connected
#endif

    //used for testing, allows tera term to set up connection
    Serial.begin(9600);
    while(!Serial);

    //unlock mutex
	os_mutex_unlock(mqtt_recv_mutex);
    os_mutex_unlock(mqtt_send_mutex);
    os_mutex_unlock(can_recv_mutex);
    os_mutex_unlock(can_send_mutex);
    os_mutex_unlock(gps_recv_mutex);
    os_mutex_unlock(dof_recv_mutex);
    //startup complete 

    RGB.control(true);         //set RGB to control
    RGB.color(255, 0, 0);       //RED shows cellular connection is valid                 
    startup=true;

}

//AWS server thread that poles for MQTT requests on different subscribed nodes
void server_thread_function(void) {
    
    while(startup == false);            //wait for startup to finish
    awsiot->connect("sparkclient");     //setup AWS connection
        if (awsiot->isConnected()) {
            awsiot->publish("outTopic/message", "hello world");
            awsiot->subscribe("inTopic/message");
        }

    RGB.color(0, 255, 0);                               //valid aws connection
    delay(1000);
	while(true) {       
        os_mutex_lock(mqtt_recv_mutex);       
        if (awsiot->isConnected()) {
            
            awsiot->loop();
        }
        os_mutex_unlock(mqtt_recv_mutex);
        os_thread_delay_until(&lastThreadTime, 10);
	} 
}

void CAN_thread_function(void){

}

void internal_thread_function(void){

}


/**
 * Constructor.
 */
CarT::CarT()
{
  // be sure not to call anything that requires hardware be initialized here, put those in begin()
}

/**
 * Example method.
 */
void CarT::begin()
{
    // initialize hardware
    Serial.println("called begin");
}

/**
 * Example method.
 */
void CarT::process()
{
    // do something useful
    Serial.println("called process");
    doit();
}

/**
* Example private method
*/
void CarT::doit()
{
    Serial.println("called doit");
}
