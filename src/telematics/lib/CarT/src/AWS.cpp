#include "AWS.h"
#include "certs.h"

AWS::AWS() {
    //add initializations for default case
}

<<<<<<< HEAD
AWS::AWS(char* domain, uint16_t port){
    setupCon(domain,port,NULL);
}

AWS::AWS(char* domain, uint16_t port, void (*callbackRef)(char*,uint8_t*,unsigned int)){
    setupCon(domain,port,callbackRef);
=======
AWS::AWS(char* domain, uint16_t port,void (*callbackRef)(char *, uint8_t *, unsigned int)){
    MQTT client(domain, port, callbackRef);
    awsMqtt = client;
>>>>>>> 15cf6e2275c96e0ebfd3ca476219a3d70fa67d24
}

bool AWS::isConnected(){
    return (awsMqtt.isConnected());
}

bool AWS::subscribe(const char* topic){
    awsMqtt.subscribe(topic);
}
bool AWS::publish(const char * topic, const char *message){
    return (awsMqtt.publish(topic, message));
}

bool AWS::subscribe(const char * topic){
    return (awsMqtt.subscribe(topic));
}

bool AWS::loop(){
    awsMqtt.loop();
}

bool AWS::connect(const char * clientID){
    return(setupCon(clientID));  
}

bool AWS::setupCon(const char* clientID) {
    bool ret=0;

    awsMqtt.enableTls(amazonIoTRootCaPem, sizeof(amazonIoTRootCaPem),
                     clientKeyCrtPem, sizeof(clientKeyCrtPem),
                     clientKeyPem, sizeof(clientKeyPem));

    ret = awsMqtt.connect(clientID);               
    return(ret);  
}
