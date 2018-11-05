#include "CAN.h"
#include "Serial5/Serial5.h"

CAN::CAN() {
    //initialization parameters
}

void CAN::transmit() 
{
    Serial5.write("stma\r");
}

void CAN::begin()
{
    Serial5.begin(9600); 
    while(!Serial5);      
    Serial5.write("atz\r");
    delay(1200);
}



    
