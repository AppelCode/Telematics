/* CarT library by CybSe@CAR
 */

#include "CarT.h"

//mqtt thread delay
system_tick_t lastThreadTime = 0;


//////////////////////////////////////////////////////////////////
//initialize thread buffers
char* mqtt_recv_buffer;     //buffer for mqqt_recv data
char* mqtt_send_buffer;     //buffer for mqtt_send data 

/*
int can_recv_buffer[RECORDS][9];   //buffer for can_recv data
int can_send_buffer[RECORDS][9];   //buffer for can_send data
float gps_recv_buffer[RECORDS][3];      //buffer for gps_recv data
///////////////////////////////////////////////////////////////////
*/

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


int temp_can_buffer[64] = {0};
float temp_dof_buffer[10] = {0};
float temp_gps_buffer[3] = {0};
/*
os_mutex_t can_recv_mutex;    
os_mutex_t can_send_mutex;    
os_mutex_t gps_recv_mutex;     
os_mutex_t dof_recv_mutex;

os_mutex_t startup_internal_mutex;
os_mutex_t startup_can_mutex;
*/
Adafruit_GPS gps = Adafruit_GPS(&Serial1);
CAN* stn = new CAN();
DOF* dof = new DOF();
SD* sd_storage = new SD();
Crypt* secretStuff = new Crypt();
AWS* awsiot = new AWS("a3mb0mz6legbs8.iot.us-east-2.amazonaws.com", 8883, callback);

//setup threads
#if MQTT_STATUS
Thread server_thread("server_thread", server_thread_function, 2,3*1024);
#endif
#if CAN_STATUS
//Thread CAN_thread("CAN_thread", CAN_thread_function, 2,9*1024);
#endif
#if DOF_STATUS || GPS_STATUS
//Thread internal_thread("Internal_thread", internal_thread_function, 2,9*1024);
#endif

//for mutex setup
void createMutexs();
void lockMutexs();
void releaseMutexs();

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

    createMutexs();
    lockMutexs();

    //setup Cellular
#if CELLULAR
    cellular_credentials_set("wireless.twilio.com", "", "", NULL);  //setup to work twilio sim
    Cellular.on();                                                  //turn on celluar modem
    Cellular.connect();                                             //connect using twillio
    while(!Cellular.ready());                                       //wait until connected
#endif
    RGB.control(true);

    releaseMutexs();       
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

void createMutexs(){

     // Create mutexs
    os_mutex_create(&mqtt_mutex);
	os_mutex_create(&mqtt_recv_mutex);
    os_mutex_create(&mqtt_send_mutex);


    //os_mutex_create(&can_recv_mutex);
    //os_mutex_create(&can_send_mutex);
    //os_mutex_create(&gps_recv_mutex);
    //os_mutex_create(&dof_recv_mutex);

    //os_mutex_create(&startup_internal_mutex);
    //os_mutex_create(&startup_can_mutex);
}
void lockMutexs(){

    //lock mutex
    os_mutex_lock(mqtt_mutex);
	os_mutex_lock(mqtt_recv_mutex);
    os_mutex_lock(mqtt_send_mutex);


    //s_mutex_lock(can_recv_mutex);
    //os_mutex_lock(can_send_mutex);
    //os_mutex_lock(gps_recv_mutex);
    //os_mutex_lock(dof_recv_mutex);

    //os_mutex_lock(startup_internal_mutex);
    //os_mutex_lock(startup_can_mutex);


}
void releaseMutexs(){
    //unlock mutex
    os_mutex_unlock(mqtt_mutex);
	os_mutex_unlock(mqtt_recv_mutex);
    os_mutex_unlock(mqtt_send_mutex);
    //os_mutex_unlock(can_recv_mutex);
    //os_mutex_unlock(can_send_mutex);
    //os_mutex_unlock(gps_recv_mutex);
    //os_mutex_unlock(dof_recv_mutex);
    os_mutex_unlock(mqtt_mutex);

    //os_mutex_unlock(startup_internal_mutex);
    //os_mutex_unlock(startup_can_mutex);
    //startup complete          
}

void internal_function(){

        //update gps
    while (Serial1.available()) {
        char c = gps.read();
        if (gps.newNMEAreceived()) {
            gps.parse(gps.lastNMEA());
        }
    }

    //dof read all 9 degrees
    dof->getAll();

    //stroe dof values in temp buffer at current record
    temp_dof_buffer[0] = dof->GX;
    temp_dof_buffer[1] = dof->GY;
    temp_dof_buffer[2] = dof->GZ;
    temp_dof_buffer[3] = dof->AX;
    temp_dof_buffer[4] = dof->AY;
    temp_dof_buffer[5] = dof->AZ;
    temp_dof_buffer[6] = dof->MX;
    temp_dof_buffer[7] = dof->MY;
    temp_dof_buffer[8] = dof->MZ;

    new_dof_flag = true;


    temp_gps_buffer[0]=gps.latitudeDegrees*pow(10,6);
    temp_gps_buffer[1]=gps.longitudeDegrees*pow(10,6);

    Serial.print(temp_gps_buffer[0]);
    Serial.print(", ");
    Serial.println(temp_gps_buffer[1]);

    if(temp_gps_buffer[0] != 0 && temp_gps_buffer[1]){
        new_gps_flag = true;
    }
    

}

void CAN_function(){

    stn->GetRPM();
    stn->receive(temp_can_buffer,0);

    new_can_flag = true;

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


/*
system_tick_t canlastThreadTime = 0;
void CAN_thread_function(void){
    os_mutex_lock(startup_can_mutex);

    int can_temp_buffer[30][9];
    int size = 0;
    int c_frames_in_buffer=0;

    stn->begin();

    while(1){

         if(!new_can_flag)
        {
            c_frames_in_buffer = 0;     //start as first frame read in buffer    
        } else {
            c_frames_in_buffer++;           //increase number of records in buffer by one    
        }  

        //lock out for writing
        stn->GetRPM();

        if(os_mutex_trylock(can_recv_mutex) && (stn->receive(can_temp_buffer[0],8) > 7))
        {
            //copy temp_dof_buffer to dof_recv_buffer
            can_frames_in_buffer = c_frames_in_buffer + 1;  //copy temp_dof_buffer to dof_recv_buffer
            for(int j = 0; j<1; j++)
            {
                memcpy(&can_recv_buffer[j], &can_temp_buffer[j], sizeof(can_temp_buffer[0]));
            }
            new_can_flag = true;                        //new information is in the recv_buffer
            os_mutex_unlock(can_recv_mutex);
        }
    }

    os_thread_delay_until(&canlastThreadTime, 10);
}
*/
/*
system_tick_t internallastThreadTime = 0;


//does not require cell connection
void internal_thread_function(void){

    //wait for startup function
    os_mutex_lock(startup_internal_mutex);

    //setup gps and dof
    dof->begin();
    while(1){

        //set number of records in temp buffer
        if(!new_dof_flag)               //if flag was tuned to false then this is first frame
        {
            d_frames_in_buffer = 0;      
        } else {
            d_frames_in_buffer++;       //increase number of records in buffer by one      
        }

        //dof read all 9 degrees
        dof->getAll();
        dof_read=true;

        //stroe dof values in temp buffer at current record
        temp_dof_buffer[d_frames_in_buffer][0] = dof->GX;
        temp_dof_buffer[d_frames_in_buffer][1] = dof->GY;
        temp_dof_buffer[d_frames_in_buffer][2] = dof->GZ;
        temp_dof_buffer[d_frames_in_buffer][3] = dof->AX;
        temp_dof_buffer[d_frames_in_buffer][4] = dof->AY;
        temp_dof_buffer[d_frames_in_buffer][5] = dof->AZ;
        temp_dof_buffer[d_frames_in_buffer][6] = dof->MX;
        temp_dof_buffer[d_frames_in_buffer][7] = dof->MY;
        temp_dof_buffer[d_frames_in_buffer][8] = dof->MZ;
        


        if(!new_gps_flag)
        {
            g_frames_in_buffer = 0;        
        } else {
            g_frames_in_buffer++;       //increase number of records in buffer by one    
        }

        //repeat for gps


        //transfer data to main buffer when locks are avaliable
#if DOF_STATUS
        if(dof_read){
            if(os_mutex_trylock(dof_recv_mutex))
            {
                //copy temp_dof_buffer to dof_recv_buffer
                dof_frames_in_buffer = d_frames_in_buffer+1;  //copy temp_dof_buffer to dof_recv_buffer
                for(int j = 0; j<dof_frames_in_buffer; j++)
                {
                    for(int i = 0; i < 9; i++){
                        dof_recv_buffer[j][i] = temp_dof_buffer[j][i];
                    }
                }
                new_dof_flag = true;                        //new information is in the recv_buffer
                os_mutex_unlock(dof_recv_mutex);
            }
        }
#endif
#if GPS_STATUS

        if(gps_read){
        if(os_mutex_trylock(gps_recv_mutex))
            {
                gps_frames_in_buffer = g_frames_in_buffer+1;  //copy temp_gps_buffer to gps_recv_buffer
                for(int j = 0; j<gps_frames_in_buffer; j++)
                {
                    memcpy(&gps_recv_buffer[j], &temp_gps_buffer[j], sizeof(temp_gps_buffer[0]));
                }
                new_gps_flag = true;                        //new information is in the recv_buffer
                os_mutex_unlock(gps_recv_mutex);
            }
        }
#endif

    os_thread_delay_until(&internallastThreadTime, 10);
   }
}
*/
