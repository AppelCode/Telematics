#include "AWS.h"
#include "Crypt.h"
#include "DOF.h"
#include "CAN.h"
#include "SD.h"
#include <Adafruit_GPS.h>
#include "cellular_hal.h"

#define RECORDS 30
#define CELLULAR true
#define SD_STATUS true
#define DOF_STATUS true
#define GPS_STATUS true
#define CAN_STATUS true
#define MQTT_STATUS true
#define CAR_ID 2

//start up and thread functions
extern void startup_function();             //startup function 
extern void server_thread_function(void);   //MQTT thread
//extern void CAN_thread_function(void);      //CAN_thread   
//extern void internal_thread_function(void); //9DOF and GPS funciton           



/////////////////////////////////////////////////////////////////
extern void CAN_function(char** pid_list,int num_pids,int &can_records);
extern unsigned char temp_can_buffer[100][16];
/////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
extern void internal_function(int &record_num, unsigned char setting);
extern float temp_dof_buffer[64][10];   //temp buffer to store dof data
extern float temp_gps_buffer[64][3];   //temp buffer to store gps data
/////////////////////////////////////////////////////////////////

extern system_tick_t lastThreadTime;        //used for thread timing if needed

//global buffers
extern char* mqtt_recv_buffer;  //buffer for mqqt_recv data
extern char* mqtt_send_buffer;  //buffer for mqtt_send data

/*
extern int can_recv_buffer[RECORDS][9];   //buffer for can_recv data
extern int can_send_buffer[RECORDS][9];   //buffer for can_send data
extern float gps_recv_buffer[RECORDS][3];   //buffer for gps_recv data
extern float dof_recv_buffer[RECORDS][10];   //buffer for dof_recv data
*/

extern bool new_can_flag;
extern bool new_dof_flag;
extern bool new_gps_flag;
extern bool new_mqtt_send_flag;

extern int can_frames_in_buffer;
extern int gps_frames_in_buffer;
extern int dof_frames_in_buffer;

//mutex locks for buffers
extern os_mutex_t mqtt_mutex;
extern os_mutex_t mqtt_recv_mutex;    
extern os_mutex_t mqtt_send_mutex;  

/*
extern os_mutex_t can_recv_mutex;    
extern os_mutex_t can_send_mutex;    
extern os_mutex_t gps_recv_mutex;     
extern os_mutex_t dof_recv_mutex;

extern os_mutex_t startup_internal_mutex;
extern os_mutex_t startup_can_mutex;
*/

//components of telematics device
//create Gps object
extern Adafruit_GPS gps;
extern CAN* stn;
extern DOF* dof;
extern SD* sd_storage;
extern Crypt* secretStuff;

extern void callback(char* topic, byte* payload, unsigned int length);
extern AWS* awsiot;


/*
*
*   work on settings for custom data collection
*
*/

extern unsigned char internal_settings;
#define DEFAULT_INTERNAL 0  //gps lat and lon and all 9dof

extern unsigned char can_settings;
#define DEFAULT_CAN 0       //rpm and speed readings

/*
 * Diagnostic PID macros
*/

#define PID_ENGINE_LOAD "0104"
#define PID_THROTTLE_POSITION "0111"
#define PID_FUEL_TANK "012F"
#define PID_MAP "010B"
#define PID_SPEED "010C"
#define PID_RPM "010D"
#define PID_TIMING_ADVANCE "010E"
#define PID_ENGINE_FUEL_RATE "015E"
#define PID_ENGINE_REFERENCE_TORQUE "0163"
#define PID_ACTUAL_GEAR "01A4"

