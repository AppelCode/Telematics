#include "CAN.h"
#include "Serial5/Serial5.h"

CAN::CAN() 
{
    //initialization parameters
}

<<<<<<< HEAD
void CAN::monitorCAN(){
    Serial5.write("STMA\r");
}

void CAN::SetHS(){
    Serial5.write("ATSP6\r");
}

void CAN::SendFrame(int header, int message){
    Serial5.write("AT"header"\r");
    Serial5.write("AT"message"\r");

}
// Creates filter to send message to 7E0 and receive from 7E8
void CAN::getRPM(){
    Serial5.write("ATCAF0\r");  // set auto format off (use all 8 bytes now)

}

// after retrieving message from specific IDs, clear filter for specific ID
void CAN::ClearFlowControl(){
    Serial5.write("STCCFCP\r");
=======
void CAN::monitorOBD() 
{
    Serial5.write("stma\r");
>>>>>>> 85fa079e0dd606f8ae7147f9815cf11c52fdd476
}

int CAN::newData()
{
    return(Serial5.available());
}
void CAN::begin()
{
    Serial5.begin(9600); 
    while(!Serial5);      
    Serial5.write("atz\r");
    delay(1200);
}
//insert buffer and size of buffer
//returns buffer with recived can data and the size of the buffer
int CAN::receive(int* buffer, int number_bytes_to_write)
{
    int temp;
    int i = 0;
    int size = Serial5.available();

    if (size >= number_bytes_to_write)
    {
        for(int i = 0; i < number_bytes_to_write; i++)
        {
            temp = Serial5.read();
            *(buffer+i) = temp;
            i++;
        }
    } else {
        return -1;
    }
    return size;
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



    
