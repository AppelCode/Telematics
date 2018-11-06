#include "AWS.h"
#include "Crypt.h"
#include "DOF.h"
#include "CAN.h"
#include "SD.h"
#include <Particle-GPS.h>
#include "cellular_hal.h"

#define CELLULAR true

//start up and thread functions
extern void startup_function();             //startup function 
extern void CAN_thread_function(void);      //CAN_thread   
extern void internal_thread_function(void); //9DOF and GPS funciton           
extern void server_thread_function(void);   //MQTT thread

extern system_tick_t lastThreadTime;        //used for thread timing if needed

//global buffers
extern char* mqtt_recv_buffer;  //buffer for mqqt_recv data
extern char* mqtt_send_buffer;  //buffer for mqtt_send data 
extern int*  can_recv_buffer;   //buffer for can_recv data
extern int*  can_send_buffer;   //buffer for can_send data
extern void* gps_recv_buffer;   //buffer for gps_recv data
extern void* dof_recv_buffer;   //buffer for dof_recv data

//mutex locks for buffers
extern os_mutex_t mqtt_recv_mutex;    
extern os_mutex_t mqtt_send_mutex;   
extern os_mutex_t can_recv_mutex;    
extern os_mutex_t can_send_mutex;    
extern os_mutex_t gps_recv_mutex;     
extern os_mutex_t dof_recv_mutex;

extern bool startup;

//components of telematics device
extern CAN* stn;
extern DOF* dof;
extern SD* sd_storage;
extern Gps* _gps;
extern Gga* gga;
extern Crypt* secretStuff;

extern void callback(char* topic, byte* payload, unsigned int length);
extern AWS* awsiot;