#ifndef AWS_H
#define AWS_H

#include <MQTT-TLS.h>

class AWS {

public:
    AWS();
    AWS(char* domain, uint16_t port,void (*callback)(char *, uint8_t *, unsigned int));

    bool isConnected();
    bool connect(const char*);
    bool publish(const char *, const char *);
    bool subscribe(const char*);
    bool loop();
    bool setupCon(const char* clientID);
    
private:

    MQTT awsMqtt;

    char* endpoint;
    int awsport;

    char payload[512];
    char rcvdpayload[512];

};
#endif