#include "CAN.h"
#include "CarT_declerations.h"
#include "Serial5/Serial5.h"


void clearBuffer(){
    while(Serial5.available())Serial5.read();
}

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
    clearBuffer();              //clear out an garbage in buffer
    Serial5.println("010C");    //request rpm data from can bus
    Serial5.flush();            //wait for request to finish
}

void CAN::GetSpeed()
{
    clearBuffer();
    Serial5.println("010D");
    Serial5.flush();

}

int CAN::GetPID(char** pid_requests,int num_requests,int current_record_count)
{
	clearBuffer();
	for(int i = 0; i < num_requests; i++)
	{
		char* current_request = pid_requests[i];
		Serial5.println(current_request);
		Serial5.flush();
		delay(50);
		stn->receive(temp_can_buffer[current_record_count],4);
		if(temp_can_buffer[current_record_count][0] != 0){
			  current_record_count++;
		}
	}
	return current_record_count;
}


int CAN::newData()
{
    return(Serial5.available());
}
void CAN::begin()
{
    Serial5.begin(9600); 
    while(!Serial5);
    Serial5.println("ATSH7DF"); //set header for diagnostic message as default
    Serial5.flush();   
    delay(200);
    Serial5.println("ATE0"); //set header for diagnostic message as default
    Serial5.flush();   
    //Serial5.write("atz\r");
    //delay(1200);
    //SetHS();
}

//insert buffer and size of buffer
//returns buffer with recived can data and the size of the buffer
int CAN::receive(unsigned char* buffer, int number_bytes_to_write)
{
    int temp;
    for (int i= 0 ; i < (number_bytes_to_write*2); i++)
    {
        temp = Serial5.read();
        
        //handle spaces in stn uart response
        if(temp == 32){
            i--;         
        } else {
            if(temp != -1){
                *(buffer+i) = (unsigned char)temp;  //store value in buffer
            }
        }
          

    }
    return 1;
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



    
