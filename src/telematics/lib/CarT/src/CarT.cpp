/* CarT library by CybSe@CAR
 */

#include "CarT.h"

system_tick_t lastThreadTime = 0;

//initialize buffers
char* mqtt_recv_buffer;     //buffer for mqqt_recv data
char* mqtt_send_buffer;     //buffer for mqtt_send data 
int**  can_recv_buffer;   //buffer for can_recv data
int**  can_send_buffer;   //buffer for can_send data
void* gps_recv_buffer[2];      //buffer for gps_recv data
float* dof_recv_buffer[9];      //buffer for dof_recv data

bool new_can_flag = false;
bool new_dof_flag = false;
bool new_gps_flag = false;

int can_frames_in_buffer = 0;
int gps_frames_in_buffer = 0;
int dof_frames_in_buffer = 0;

//mutex locks for buffers
os_mutex_t mqtt_mutex;
os_mutex_t mqtt_recv_mutex;    
os_mutex_t mqtt_send_mutex;   
os_mutex_t can_recv_mutex;    
os_mutex_t can_send_mutex;    
os_mutex_t gps_recv_mutex;     
os_mutex_t dof_recv_mutex;

CAN* stn = new CAN();
DOF* dof = new DOF();
SD* sd_storage = new SD();
Crypt* secretStuff = new Crypt();
Gps* _gps = new Gps(&Serial1);
//Gga* gga = new Gga(*_gps);
AWS* awsiot = new AWS("a3mb0mz6legbs8.iot.us-east-2.amazonaws.com", 8883, callback);

//setup threads
#if MQTT_STATUS
Thread server_thread("server_thread", server_thread_function, OS_THREAD_PRIORITY_DEFAULT,6*1024);
#endif
#if CAN_STATUS
Thread CAN_thread("CAN_thread", CAN_thread_function, OS_THREAD_PRIORITY_DEFAULT,3*1024);
#endif
#if DOF_STATUS || GPS_STATUS
Thread internal_thread("Internal_thread", internal_thread_function, OS_THREAD_PRIORITY_DEFAULT,3*1024);
#endif

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
    os_mutex_create(&mqtt_mutex);
	os_mutex_create(&mqtt_recv_mutex);
    os_mutex_create(&mqtt_send_mutex);
    os_mutex_create(&can_recv_mutex);
    os_mutex_create(&can_send_mutex);
    os_mutex_create(&gps_recv_mutex);
    os_mutex_create(&dof_recv_mutex);

    //lock mutex
    os_mutex_lock(mqtt_mutex);
	os_mutex_lock(mqtt_recv_mutex);
    os_mutex_lock(mqtt_send_mutex);
    os_mutex_lock(can_recv_mutex);
    os_mutex_lock(can_send_mutex);
    os_mutex_lock(gps_recv_mutex);
    os_mutex_lock(dof_recv_mutex);

    //setup Cellular
#if CELLULAR
    cellular_credentials_set("wireless.twilio.com", "", "", NULL);  //setup to work twilio sim
    Cellular.on();                                                  //turn on celluar modem
    Cellular.connect();                                             //connect using twillio
    while(!Cellular.ready());                                       //wait until connected
#endif
    RGB.control(true);

    //unlock mutex
	os_mutex_unlock(mqtt_recv_mutex);
    os_mutex_unlock(mqtt_send_mutex);
    os_mutex_unlock(can_recv_mutex);
    os_mutex_unlock(can_send_mutex);
    os_mutex_unlock(gps_recv_mutex);
    os_mutex_unlock(dof_recv_mutex);
    //startup complete               
}

//AWS server thread that poles for MQTT requests on different subscribed nodes
//requires cell connection
void server_thread_function(void) {
  
    while(!Cellular.ready())
    awsiot->connect("sparkclient");     //setup AWS connection
        if (awsiot->isConnected()) {
            awsiot->publish("outTopic/message", "hello world");
            awsiot->subscribe("inTopic/message");
        }
    os_mutex_unlock(mqtt_mutex);
	while(true) { 
        //check for any new recieve messages      
        os_mutex_lock(mqtt_recv_mutex);       
        if (awsiot->isConnected()) {
            
            awsiot->loop();
        }
        os_mutex_unlock(mqtt_recv_mutex);

        //publish new message
        os_mutex_lock(mqtt_send_mutex);
        if (strlen(mqtt_send_buffer) > 0){
            awsiot->publish("cart/2",mqtt_send_buffer);
        }   
        os_mutex_unlock(mqtt_send_mutex);    

        os_thread_delay_until(&lastThreadTime, 10);
	} 

}

//does not require cell connection
void CAN_thread_function(void){
    int buffer[8];
    int size = 0;
    int current_frames;

    stn->begin();

    while(1){

        //lock out for writing
        os_mutex_lock(can_recv_mutex); 
        current_frames = stn->newData();
        if (current_frames!=0 && current_frames%8==0)
        {   
            //if data has been read clear buffer
            if(!new_can_flag){
                memset(can_recv_buffer,0,RECORDS);
                can_frames_in_buffer = 0;
            }

            //fill can_recv_buffer
            do
            {      
                size = stn->receive(buffer,8);                              //add next data frame into temp buffer
                memcpy(can_recv_buffer[can_frames_in_buffer],buffer,8);    //add to recvbuffer
                can_frames_in_buffer ++;                                    //increase number of frames in buffer     
            }
            while (size%8==0);
            new_can_flag = true;
        }
        os_mutex_unlock(can_recv_mutex);
    }
}

//does not require cell connection
void internal_thread_function(void){

    //setup gps and dof
    _gps->begin(9600);   //setup GPS   
    dof->begin();        //DOF begin 

    float* temp_dof_buffer[9];
    dof_frames_in_buffer = 0;
    //never return
    while(1){

        //set number of records in temp buffer
        if(!new_dof_flag)
        {
            dof_frames_in_buffer = 1;   //start as first frame read in buffer    
        } 
        else
        {  
            dof_frames_in_buffer++;      //increase number of records in buffer by one        
        }
        //re allocate memory based on number records
        realloc(temp_dof_buffer,sizeof(float*)*dof_frames_in_buffer);

        //dof read all 9 degrees
        dof->getAll();
        float temp[9] = {dof->GX,dof->GY,dof->GZ,dof->AX,dof->AY,dof->AZ,dof->MX,dof->MY,dof->MZ};
        memcpy(temp_dof_buffer[dof_frames_in_buffer-1],temp,9);

        //if dof_buffer not in use write to it, if it is then grab another record
        if(os_mutex_trylock(dof_recv_mutex))
        {
            //copy temp_dof_buffer to dof_recv_buffer
            memcpy(dof_recv_buffer,temp_dof_buffer,dof_frames_in_buffer);
            new_dof_flag = true;
            os_mutex_lock(dof_recv_mutex);
        }
   }

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
