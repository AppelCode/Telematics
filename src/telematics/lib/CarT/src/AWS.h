#ifndef AWS_H
#define AWS_H

#include <MQTT-TLS.h>

class AWS {

public:
    AWS();
    AWS(char* domain, uint16_t port,void (*callbackRef)(char *, uint8_t *, unsigned int));

    bool isConnected();
    bool connect(const char*);
    bool publish(const char *, const char *);
    bool subscribe(const char*);
    bool loop();
    bool setupCon(char* domain, uint16_t port, void (*callbackRef)(char*,uint8_t*,unsigned int));
    
private:

    MQTT awsMqtt;
    const char* clientID;

    char* endpoint;
    int awsport;

    char payload[512];
    char rcvdpayload[512];

};
#endif