#include "AWS.h"

AWS::AWS() {
    //add initializations for default case
}

AWS::AWS(char* domain, uint16_t port){
    setupCon(domain,port,NULL);
}

AWS::AWS(char* domain, uint16_t port, void (*callbackRef)(char*,uint8_t*,unsigned int)){
    setupCon(domain,port,callbackRef);
}

bool AWS::isConnected(){
    return (awsMqtt->isConnected());
}

bool AWS::subscribe(const char* topic){
    awsMqtt.subscribe(topic);
}
bool AWS::publish(const char * topic, const char *message){
    return (awsMqtt->publish(topic, message));
}

bool AWS::connect(const char * clinetID){
    if(awsMqtt->enableTls(amazonIoTRootCaPem, sizeof(amazonIoTRootCaPem),
                     clientKeyCrtPem, sizeof(clientKeyCrtPem),
                     clientKeyPem, sizeof(clientKeyPem))){
                        return (awsMqtt->connect("sparkclient"));  
                     }
                     else {
                         return false;
                     }
    
}

void AWS::setupCon(char* domain, uint16_t port, void (*callbackRef)(char*,uint8_t*,unsigned int)) {
    static MQTT client(domain, 8883, callback);
    awsMqtt = &client;  
}