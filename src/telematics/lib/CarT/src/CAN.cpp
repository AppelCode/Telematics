#include "CAN.h"
#include "Serial5/Serial5.h"

CAN::CAN() 
{
    //initialization parameters
}

void CAN::monitorCAN(){
    Serial5.write("STMA\r");
}

void CAN::SetHS(){
    Serial5.write("ATSP6\r");
}

void CAN::SendFrame(int header, int message){
}

// after retrieving message from specific IDs, clear filter for specific ID
void CAN::ClearFlowControl(){
    Serial5.write("STCCFCP\r");
}

void CAN::GetRPM()
{
    
    Serial5.println("ATSH7DF");
    Serial5.flush();
    Serial5.println("010C");
    Serial5.flush();
}

void CAN::GetSpeed()
{
    Serial5.write("ATSH7DF\r");
    Serial5.write("010D\r");

}


int CAN::newData()
{
    return(Serial5.available());
}
void CAN::begin()
{
    Serial5.begin(9600); 
    while(!Serial5);      
    //Serial5.write("atz\r");
    //delay(1200);
    //SetHS();
}
//insert buffer and size of buffer
//returns buffer with recived can data and the size of the buffer
int CAN::receive(unsigned char* buffer, int number_bytes_to_write)
{
    int temp;
    int i = 0;
    int size = Serial5.available();

    for (int i= 0 ; i < (number_bytes_to_write*2 + (number_bytes_to_write-1)); i++)
    {
        if((i+1)%3 == 0){
            Serial.read();
        } else {
           temp = Serial5.read();
            *(buffer+i) = (unsigned char)temp; 
        }
    }
    while(Serial5.available());
    return i-1;
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



    
