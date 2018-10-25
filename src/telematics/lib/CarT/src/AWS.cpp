#include "AWS.h"
#include "certs.h"

AWS::AWS() {
    //add initializations for default case
}

AWS::AWS(char* domain, uint16_t port, void (*callbackRef)(char*,uint8_t*,unsigned int)){
    setupCon(domain,port,callbackRef);
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
