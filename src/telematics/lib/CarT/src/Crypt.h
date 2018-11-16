#ifndef CRYPT_H
#define CRYPT_H

#include <MQTT-TLS.h>


class Crypt {
    public:

        Crypt();
        int generateKey();
        int encryptData(unsigned char* input,unsigned char* output);
        int decryptData(unsigned char* input,unsigned char* output);
        
        char*pers = "CyberAuto";

        mbedtls_aes_context aes_in;
        mbedtls_aes_context aes_out;

    private:
        unsigned char key[32]={ 'C', 12, 102, 74, 1, 99, 'y', 'b',\
         12, 36, 'S', 53, 2, 5, 34, 'e', 82, 91, 'C', 43, 110, 0, 55, 100, 'A', 13, 12, 45, 29, 110, 'R' };
        unsigned char iv[16];
        unsigned char ogiv[16] = { 14, 31, 6, 126, 18, 12, 36, 70, 100, 9, 42, 51, 111, 84, 3, 25 };

};
#endif 