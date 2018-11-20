#include "AWS.h"
#include "Crypt.h"
#include "DOF.h"
#include "CAN.h"
#include "SD.h"
#include <Adafruit_GPS.h>
#include "cellular_hal.h"

#define RECORDS 30
#define CELLULAR true
#define SD_STATUS false
#define DOF_STATUS false
#define GPS_STATUS false
#define CAN_STATUS false
#define MQTT_STATUS true
#define CAR_ID 2

//start up and thread functions
extern void startup_function();             //startup function 
extern void CAN_thread_function(void);      //CAN_thread   
extern void internal_thread_function(void); //9DOF and GPS funciton           
extern void server_thread_function(void);   //MQTT thread

extern system_tick_t lastThreadTime;        //used for thread timing if needed

//global buffers
extern char* mqtt_recv_buffer;  //buffer for mqqt_recv data
extern char* mqtt_send_buffer;  //buffer for mqtt_send data 
extern int can_recv_buffer[RECORDS][9];   //buffer for can_recv data
extern int can_send_buffer[RECORDS][9];   //buffer for can_send data
extern float gps_recv_buffer[RECORDS][3];   //buffer for gps_recv data
extern float dof_recv_buffer[RECORDS][10];   //buffer for dof_recv data

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
extern os_mutex_t can_recv_mutex;    
extern os_mutex_t can_send_mutex;    
extern os_mutex_t gps_recv_mutex;     
extern os_mutex_t dof_recv_mutex;

extern os_mutex_t startup_internal_mutex;
extern os_mutex_t startup_can_mutex;

extern bool startup;

//components of telematics device
extern CAN* stn;
extern DOF* dof;
extern SD* sd_storage;
extern Crypt* secretStuff;

extern void callback(char* topic, byte* payload, unsigned int length);
extern AWS* awsiot;