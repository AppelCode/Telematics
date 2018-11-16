#include "Crypt.h"
#include "application.h"
Crypt::Crypt(){

}


//generate key
int Crypt::generateKey(){
    int ret=0;

    mbedtls_aes_init(&aes_in);
    mbedtls_aes_init(&aes_out);
    memcpy(iv,ogiv,16);
    
    mbedtls_aes_setkey_enc( &aes_in, key, 256 );
    mbedtls_aes_setkey_dec( &aes_out, key, 256 );

    return ret;
}

//encrypt stuff
int Crypt::encryptData(unsigned char* input,unsigned char* output){
    int ret=0;
    
    ret = mbedtls_aes_crypt_cbc( &aes_in, MBEDTLS_AES_ENCRYPT, 128, iv, input, output );

    return ret;
}

//decrypt stuff
int Crypt::decryptData(unsigned char* input,unsigned char* output){
    int ret=0;

    ret = mbedtls_aes_crypt_cbc( &aes_out, MBEDTLS_AES_DECRYPT, 128, ogiv, input, output );  

    return ret;

}