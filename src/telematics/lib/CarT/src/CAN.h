#ifndef CAN_H
#define CAN_H

#include "Particle.h"
class CAN
{
    public:
        CAN();
        void begin();
<<<<<<< HEAD
        void monitorCAN();
        void getRPM();
        void ClearFlowControl();
        void SetHS();
        void SendFrame(int header, int message);
        int receive(int* buffer, int& size);
=======
        void monitorOBD();
        int newData();
        int receive(int* buffer,int number_bytes_to_write);
>>>>>>> 85fa079e0dd606f8ae7147f9815cf11c52fdd476
    private:
};
#endif
