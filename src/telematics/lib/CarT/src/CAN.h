#ifndef CAN_H
#define CAN_H

#include "Particle.h"
class CAN
{
    public:
        CAN();
        void begin();
<<<<<<< HEAD
        void monitorOBD();
        int receive(int* buffer, int& size);
=======
        void transmit();
        char receive();
>>>>>>> 06338b0451a1fed0ad6113047f5c32284c2fcdfc
    private:
};
#endif
