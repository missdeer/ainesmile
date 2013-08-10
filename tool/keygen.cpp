/*************************************************************************
	> File Name: keygen.cpp
	> Author: Fan Yang
	> Mail: missdeer@dfordsoft.com
 ************************************************************************/

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/bn.h>
#include <openssl/crypto.h>
using namespace std;

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        cout << "usage:\n"
            << "  keygen private.key username pincode"
            << endl;
        return 1;
    }

    char * keyfile = argv[1];

    char * username = argv[2];
    int username_len = strlen(username);
    char * pincode = argv[3];
    int pincode_len = strlen(pincode);
    unsigned char *text = new unsigned char[username_len + pincode_len + 1];
    unsigned char *p = text;
    memset(text, 0, username_len + pincode_len + 1);
    for(size_t i = 0; i < (username_len > pincode_len ? username_len : pincode_len); i++)
    {
        if (i < username_len)
            *(p++) = username[i];
        if (i < pincode_len)
            *(p++) = pincode[i];
    }

    FILE* fp = fopen(keyfile, "r");
    if (fp)
    {
        RSA* rsa = PEM_read_RSAPrivateKey(fp, NULL, NULL, NULL);
        int rsa_len = RSA_size(rsa);
        unsigned char * encrypt_text = new unsigned char[rsa_len + 1];
        memset(encrypt_text, 0, rsa_len + 1);
        RSA_private_encrypt(username_len + pincode_len, text, encrypt_text, rsa, RSA_PKCS1_PADDING);

        //cout << text << endl;
        cout << encrypt_text << endl;

        delete[] encrypt_text;
        fclose(fp);
    }
    delete[] text;
    return 0;
}

