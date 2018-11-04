#include "Crypt.h"
#include "application.h"

//generate key
int Crypt::generateKey(unsigned char* key){
    int ret=0;

    mbedtls_entropy_init( &entropy );
    mbedtls_ctr_drbg_init( &ctr_drbg );
    mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy,(unsigned char *) pers, strlen(pers) );
    mbedtls_ctr_drbg_random( &ctr_drbg, key, 32 );

    return ret;
}

//encrypt stuff
int Crypt::encryptData(unsigned char* key, unsigned char* input,unsigned char* output){
    int ret=0;
    
    mbedtls_aes_setkey_enc( &aes, key, 256 );
    mbedtls_aes_crypt_cbc( &aes, MBEDTLS_AES_ENCRYPT, 48, iv, input, output );

    return ret;
}

//decrypt stuff
int Crypt::decryptData(unsigned char* key, unsigned char* input,unsigned char* output){
    int ret=0;

    mbedtls_aes_crypt_cbc( &aes, MBEDTLS_AES_DECRYPT, 48, iv, input, output );  

    return ret;

}