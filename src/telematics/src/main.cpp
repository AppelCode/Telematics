// This #include statement was automatically added by the Particle IDE.
#include "Particle.h"
#include <MQTT-TLS.h>
#include <CarT.h>

//setup threading
SYSTEM_THREAD(ENABLED);
SYSTEM_MODE(SEMI_AUTOMATIC);

void startupFunction();
void threadFunction(void *param);

//setup mutex
STARTUP(startupFunction());

//mutex to be used to block thread untill needed
os_mutex_t mutex;
Thread thread("testThread", threadFunction);

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

#define ONE_DAY_MILLIS (24 * 60 * 60 * 1000)
unsigned long lastSync = millis();
int counter = 0;
void setup() {
    //resync time everyday
    if (millis() - lastSync > ONE_DAY_MILLIS) {
        Particle.syncTime();
        lastSync = millis();
    }


    RGB.control(true);

}

void loop() {
    if (awsiot.isConnected()) {
        awsiot.loop();
    }
    delay(200);
}

//use for thread setup
//use for connection setup
void startupFunction() {
	
    awsiot.connect("sparkclient");

    // publish/subscribe
    if (awsiot.isConnected()) {
        Serial.println("client connected");
        awsiot.publish("outTopic/message", "hello world");
        awsiot.subscribe("inTopic/message");
    }

    // Create the mutex
	os_mutex_create(&mutex);

	// Initially lock it, so when the thread tries to lock it, it will block.
	// It's unlocked in buttonHandler()
	os_mutex_lock(mutex);
}

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