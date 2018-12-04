#ifndef CART_SD_H
#define CART_SD_H

#include <SdFat.h>
#include "Particle.h"


#define INT_TYPE 0
#define FLOAT_TYPE 1

#define SPI_CONFIGURATION 0
const uint8_t chipSelect = A0;


class SD
{
public:
    SD();
    bool begin();
    bool write(const char* data);
    void read();

private:

    File myFile;
    SdFat sd;
    int data;
};
#endif

#include "SD_template_functions.h"