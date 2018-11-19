#ifndef CAN_H
#define CAN_H

#include "Particle.h"
class CAN
{
    public:
        CAN();
        void begin();
        void monitorCAN();
        void getRPM();
        void ClearFlowControl();
        void SetHS();
        void SendFrame(int header, int message);
        int receive(int* buffer, int& size);
    private:
};
#endif
