// This #include statement was automatically added by the Particle IDE.
#include "Particle.h"
#include <MQTT-TLS.h>
#include <CarT.h>

void callback(char* topic, byte* payload, unsigned int length);

/**
 * if want to use IP address,
 * byte server[] = { XXX,XXX,XXX,XXX };
 * MQTT client(server, 1883, callback);
 * want to use domain name,
 * MQTT client("www.sample.com", 1883, callback);
 **/
AWS awsThing("a3mb0mz6legbs8.iot.us-east-2.amazonaws.com", 8883, &callback);

// recieve message
void callback(char* topic, byte* payload, unsigned int length) {
    //to be defined will execute in another thread eventually
}

#define ONE_DAY_MILLIS (24 * 60 * 60 * 1000)
unsigned long lastSync = millis();
int counter = 0;
void setup() {

    RGB.control(true);
    //connect to aws
    awsThing.connect("CarT");
    // publish/subscribe
    if (awsThing.isConnected()) {
        Serial.println("client connected");
        awsThing.publish("outTopic/message", "hello world");
    }
}

void loop() {
    delay(200);
}