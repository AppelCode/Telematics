/* CarT library by CybSe@CAR
 */

#include "CarT.h"

system_tick_t lastThreadTime = 0;

//initialize buffers
char* mqtt_recv_buffer;     //buffer for mqqt_recv data
char* mqtt_send_buffer;     //buffer for mqtt_send data 
int can_recv_buffer[RECORDS][8];   //buffer for can_recv data
int can_send_buffer[RECORDS][8];   //buffer for can_send data
float gps_recv_buffer[RECORDS][2];      //buffer for gps_recv data
float dof_recv_buffer[RECORDS][9];      //buffer for dof_recv data

bool new_can_flag = false;
bool new_dof_flag = false;
bool new_gps_flag = false;
bool new_mqtt_send_flag = false;

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

os_mutex_t startup_internal_mutex;
os_mutex_t startup_can_mutex;

CAN* stn = new CAN();
DOF* dof = new DOF();
SD* sd_storage = new SD();
Crypt* secretStuff = new Crypt();
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

    os_mutex_create(&startup_internal_mutex);
    os_mutex_create(&startup_can_mutex);

    //lock mutex
    os_mutex_lock(mqtt_mutex);
	os_mutex_lock(mqtt_recv_mutex);
    os_mutex_lock(mqtt_send_mutex);
    os_mutex_lock(can_recv_mutex);
    os_mutex_lock(can_send_mutex);
    os_mutex_lock(gps_recv_mutex);
    os_mutex_lock(dof_recv_mutex);

    os_mutex_lock(startup_internal_mutex);
    os_mutex_lock(startup_can_mutex);

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
    os_mutex_unlock(mqtt_mutex);

    os_mutex_unlock(startup_internal_mutex);
    os_mutex_unlock(startup_can_mutex);
    //startup complete               
}

//AWS server thread that poles for MQTT requests on different subscribed nodes
//requires cell connection
void server_thread_function(void) {
  
    while(!Cellular.ready());           //wait for cell connection
    awsiot->connect("sparkclient");     //setup AWS connection
        if (awsiot->isConnected()) {        
            awsiot->publish("outTopic/message", "hello world"); //send hello world confirmation
            awsiot->subscribe("inTopic/message");               //subscribe to topic to recv messages
        }   
	while(true) { 
        //check for any new recieve messages      
        os_mutex_lock(mqtt_mutex);                 //grab lock    
        if (awsiot->isConnected()) {               //check for connection     
            awsiot->loop();                        //look for any received messages
        }
        os_mutex_unlock(mqtt_mutex);               //release lock

        os_thread_delay_until(&lastThreadTime, 10);     //delay thread
	} 

}

void CAN_thread_function(void){
    os_mutex_lock(startup_can_mutex);

    int can_temp_buffer[30][8];
    int size = 0;
    int current_frames;
    int c_frames_in_buffer=0;

    stn->begin();

    while(1){

         if(!new_can_flag)
        {
            c_frames_in_buffer = 0;     //start as first frame read in buffer    
        } 
        c_frames_in_buffer++;           //increase number of records in buffer by one      

        //lock out for writing
        stn->GetRPM();

        if(os_mutex_trylock(can_recv_mutex) && stn->receive(can_temp_buffer[0],8))
        {
            //copy temp_dof_buffer to dof_recv_buffer
            can_frames_in_buffer = 1;  //copy temp_dof_buffer to dof_recv_buffer
            for(int j = 0; j<1; j++)
            {
                memcpy(&can_recv_buffer[j], &can_temp_buffer[j], sizeof(can_temp_buffer[0]));
            }
            new_can_flag = true;                        //new information is in the recv_buffer
            os_mutex_unlock(can_recv_mutex);
        }
    }
}

//does not require cell connection
void internal_thread_function(void){

    //wait for startup function
    os_mutex_lock(startup_internal_mutex);

    //setup gps and dof
    dof->begin();                   //DOF begin communication
    float temp_dof_buffer[30][9];   //temp buffer to store dof data
    float temp_gps_buffer[30][2];   //temp buffer to store gps data
    dof_frames_in_buffer = 0;       //set frames to 0, ie no records yet
    gps_frames_in_buffer = 0;       //set frames to 0, ie no records yet
    int d_frames_in_buffer = 0;     //set frames to 0, ie no records yet
    int g_frames_in_buffer = 0; //set frames to 0, ie no records yet
    //never return
    while(1){

        //set number of records in temp buffer
        if(!new_dof_flag)
        {
            d_frames_in_buffer = 0;     //start as first frame read in buffer    
        } 
        d_frames_in_buffer++;           //increase number of records in buffer by one      

        if(!new_gps_flag)
        {
            g_frames_in_buffer = 0;     //start as first frame read in buffer    
        } 
        g_frames_in_buffer++;           //increase number of records in buffer by one    

        //dof read all 9 degrees
        dof->getAll();

        //stroe dof values in temp buffer at current record
        temp_dof_buffer[d_frames_in_buffer-1][0] = 13.4;
        temp_dof_buffer[d_frames_in_buffer-1][1] = 13.4;
        temp_dof_buffer[d_frames_in_buffer-1][2] = 13.4;
        temp_dof_buffer[d_frames_in_buffer-1][3] = 13.4;
        temp_dof_buffer[d_frames_in_buffer-1][4] = 13.4;
        temp_dof_buffer[d_frames_in_buffer-1][5] = 13.4;
        temp_dof_buffer[d_frames_in_buffer-1][6] = 13.4;
        temp_dof_buffer[d_frames_in_buffer-1][7] = 13.4;
        temp_dof_buffer[d_frames_in_buffer-1][8] = 13.4;

        //repeat for gps


        //transfer data to main buffer when locks are avaliable
#if DOF_STATUS
        if(os_mutex_trylock(dof_recv_mutex))
        {
            //copy temp_dof_buffer to dof_recv_buffer
            dof_frames_in_buffer = d_frames_in_buffer;  //copy temp_dof_buffer to dof_recv_buffer
            for(int j = 0; j<dof_frames_in_buffer; j++)
            {
                memcpy(&dof_recv_buffer[j], &temp_dof_buffer[j], sizeof(temp_dof_buffer[0]));
            }
            new_dof_flag = true;                        //new information is in the recv_buffer
            os_mutex_unlock(dof_recv_mutex);
        }
#endif
#if GPS_STATUS

        if(os_mutex_trylock(gps_recv_mutex))
        {
            gps_frames_in_buffer = g_frames_in_buffer;  //copy temp_gps_buffer to gps_recv_buffer
            for(int j = 0; j<gps_frames_in_buffer; j++)
            {
                memcpy(&gps_recv_buffer[j], &temp_gps_buffer[j], sizeof(temp_gps_buffer[0]));
            }
            new_dof_flag = true;                        //new information is in the recv_buffer
            os_mutex_unlock(gps_recv_mutex);
        }
#endif       
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
