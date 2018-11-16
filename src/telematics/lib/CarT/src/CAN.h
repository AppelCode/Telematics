#ifndef CAN_H
#define CAN_H

#include "Particle.h"
class CAN
{
    public:
        CAN();
        void begin();
        void monitorOBD();
        int newData();
        int receive(int* buffer,int number_bytes_to_write);
    private:
};
#endif
