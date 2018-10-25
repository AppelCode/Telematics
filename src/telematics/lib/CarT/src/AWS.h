#ifndef AWS_H
#define AWS_H

#include <MQTT-TLS.h>
#include "certs.h"


class AWS {

public:
    AWS();
    AWS(char* domain, uint16_t port);
    AWS(char* domain, uint16_t port, void (*callbackRef)(char*,uint8_t*,unsigned int));

    bool isConnected();
    bool connect(const char*);
    bool publish(const char *, const char *);
    bool subscribe(const char*);
    void setupCon(char* domain, uint16_t port, void (*callbackRef)(char*,uint8_t*,unsigned int));
    
private:

    MQTT* awsMqtt;

    char payload[512];
    char rcvdpayload[512];

};
#endif