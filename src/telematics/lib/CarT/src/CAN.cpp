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

//insert buffer and size of buffer
//returns buffer with recived can data and the size of the buffer
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

/*
char CAN::receive(){
    char rxData[32];
    char rxIndex = 0;
    char n = 0;
    while(n != '\r'){
        if (Serial5.available() > 0){
                if (Serial5.peak() == '\r'){
                    n = Serial5.read();
                    rxData[rxIndex] = '\0';
                    rxIndex = 0;
                }
        }
        else {
            n = Serial5.read();
            rxData[rxIndex++] = n;
        }
    }
    return rxData;      
 }
 */



    
