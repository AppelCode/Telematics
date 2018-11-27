#ifndef CAN_H
#define CAN_H

#include "Particle.h"
class CAN
{
    public:
        CAN();
        void begin();
        void SetHS();
        void GetRPM();
        void GetSpeed();
        void monitorCAN();
        void ClearFlowControl();
        void SendFrame(int header, int message);
        int receive(unsigned char*, int);
        int newData();
    private:
};
#endif
