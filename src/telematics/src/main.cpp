// This #include statement was automatically added by the Particle IDE.
#include "Particle.h"
#include "cellular_hal.h"
#include <MQTT-TLS.h>
#include <CarT.h>
#include <SdFat.h>

//setup threading
SYSTEM_THREAD(ENABLED);
SYSTEM_MODE(MANUAL);

void startupFunction();
void threadFunction(void *param);

//setup mutex
STARTUP(startupFunction());

//mutex to be used to block thread untill needed
os_mutex_t mutex;
//Thread thread("testThread", threadFunction);

//#include <CarT.h>
void callback(char* topic, byte* payload, unsigned int length);

/**
 * if want to use IP address,
 * byte server[] = { XXX,XXX,XXX,XXX };
 * MQTT client(server, 1883, callback);
 * want to use domain name,
 * MQTT client("www.sample.com", 1883, callback);
 **/

AWS awsiot("a3mb0mz6legbs8.iot.us-east-2.amazonaws.com", 8883, callback);
Crypt secretStuff;

//DOF dof;

unsigned char key[32];

// recieve message
//used for handling all subscription messages
//will be updated using for updating rec buffer
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
// dom add below

// Pick an SPI configuration.
// See SPI configuration section below (comments are for photon).
#define SPI_CONFIGURATION 0
//------------------------------------------------------------------------------
// Setup SPI configuration.
#if SPI_CONFIGURATION == 0
// Primary SPI with DMA
// SCK => A3, MISO => A4, MOSI => A5, SS => A2 (default)
SdFat sd;
const uint8_t chipSelect = SS;

#endif  // SPI_CONFIGURATION

File myFile;

// dom add above

#define ONE_DAY_MILLIS (24 * 60 * 60 * 1000)
unsigned long lastSync = millis();
int counter = 0;
void setup() {


    Serial.begin(9600);
    while (!Serial) {
    SysCall::yield();
    }

    /* Initialize SdFat or print a detailed error message and halt
    *  Use half speed like the native library.
    *  Change to SPI_FULL_SPEED for more performance.
    */
    if (!sd.begin(chipSelect, SPI_HALF_SPEED)) {
        sd.initErrorHalt();
    }

    /*
    //resync time everyday
    if (millis() - lastSync > ONE_DAY_MILLIS) {
        Particle.syncTime();
        lastSync = millis();
    }
    */
    dof.getTemp();
    float temp = dof.TEMP;
    awsiot.connect("sparkclient");



    // publish/subscribe
    if (awsiot.isConnected()) {
        Serial.println("client connected");
        awsiot.publish("outTopic/message", "hello world");
        awsiot.subscribe("inTopic/message");
    }

    RGB.control(true);

    secretStuff.generateKey(key);
    Serial.println((int)key);

    // open the file for write at end like the "Native SD library"
    if (!myFile.open("test.txt", O_RDWR | O_CREAT | O_AT_END)) {
        sd.errorHalt("opening test.txt for write failed");
    }
    // if the file opened okay, write to it:
    //Serial.print("Writing to test.txt...");
    myFile.println(temp);
    myFile.printf("fileSize: %d\n", myFile.fileSize());

    // close the file:
    myFile.close();
}   

void loop() {

    if (awsiot.isConnected()) {
        Serial.println("i made it");
        awsiot.loop();
    }

    
    delay(200);
}

//use for thread setup
//use for connection setup
void startupFunction() {
    
    cellular_credentials_set("wireless.twilio.com", "", "", NULL);
    Cellular.on(); 
    Cellular.connect();

    while(!Cellular.ready()){
        Cellular.connect();
    }
    

    // Create the mutex
	os_mutex_create(&mutex);

	// Initially lock it, so when the thread tries to lock it, it will block.
	os_mutex_lock(mutex);
}

/*
void threadFunction(void *param) {
	while(true) {
		// Block until unlocked by the buttonHandler
		os_mutex_lock(mutex);

		WITH_LOCK(Serial) {
			Serial.println("thread called!");
            awsiot.loop();
		}
	}
	// You must not return from the thread function
}
*/