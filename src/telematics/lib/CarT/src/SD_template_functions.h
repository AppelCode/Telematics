#pragma once

#ifndef CART_SD_TEMPLATE_H
#define CART_SD_TEMPLATE_H

#include "SD.h"
template<typename datatype>
bool SD::write(datatype data)
    {
        // open the file for write 
        if (!myFile.open("test.txt", O_RDWR | O_CREAT | O_AT_END)) {
            return false;
        }
        
        myFile.print(data);   // write data to Sd
        myFile.close();         // close file to save data

        return true;
    }
#endif