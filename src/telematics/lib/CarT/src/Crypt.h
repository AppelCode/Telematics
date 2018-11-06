#ifndef CRYPT_H
#define CRYPT_H

#include <MQTT-TLS.h>


class Crypt {
    public:

        Crypt();
        int generateKey(unsigned char* key);
        int encryptData(unsigned char* input,unsigned char* output);
        int decryptData(unsigned char* input,unsigned char* output);

    private:
        //used for key generation
        mbedtls_ctr_drbg_context ctr_drbg;
        mbedtls_entropy_context entropy;
        const char *pers = "CyberAuto";

        //used for encryption
        mbedtls_aes_context aes;
        unsigned char iv[16];

};
#endif 