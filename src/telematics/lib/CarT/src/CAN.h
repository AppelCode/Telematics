#ifndef CAN_H
#define CAN_H

#include "Particle.h"
class CAN
{
    public:
        CAN();
        void begin();
        void monitorOBD();
        int receive(int* buffer, int& size);
    private:
};
#endif