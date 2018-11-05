#include "CAN.h"
#include "Serial5/Serial5.h"

CAN::CAN() {
    //initialization parameters
}

void CAN::monitorOBD() {
    Serial5.write("stma\r");
}

void CAN::begin(){
    Serial5.begin(9600); 
    while(!Serial5);      
    Serial5.write("atz\r");
    delay(1200);
}

int CAN::receive(int* buffer, int& size){
    int temp;
    int i = 0;

    while(Serial5.available())
    {
        temp = Serial5.read();
        *(buffer+i) = temp;
        i++;
    }
    size = i;
    return Serial5.available();
}





    