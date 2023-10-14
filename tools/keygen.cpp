/*************************************************************************
	> File Name: keygen.cpp
	> Author: Fan Yang
	> Mail: missdeer@dfordsoft.com
 ************************************************************************/

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <string>
#include <cassert>
#include <limits>
#include <stdexcept>
#include <cctype>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/bn.h>
#include <openssl/crypto.h>
using namespace std;

static const char b64_table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const char reverse_table[128] = {
   64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
   64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
   64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
   52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
   64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
   15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
   64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
   41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64
};

std::string base64_encode(const std::string &bindata)
{
   using std::string;
   using std::numeric_limits;

   if (bindata.size() > (numeric_limits<string::size_type>::max() / 4u) * 3u) {
      throw std::length_error("Converting too large a string to base64.");
   }

   const std::size_t binlen = bindata.size();
   // Use = signs so the end is properly padded.
   string retval((((binlen + 2) / 3) * 4), '=');
   std::size_t outpos = 0;
   int bits_collected = 0;
   unsigned int accumulator = 0;
   const string::const_iterator binend = bindata.end();

   for (string::const_iterator i = bindata.begin(); i != binend; ++i) {
      accumulator = (accumulator << 8) | (*i & 0xffu);
      bits_collected += 8;
      while (bits_collected >= 6) {
         bits_collected -= 6;
         retval[outpos++] = b64_table[(accumulator >> bits_collected) & 0x3fu];
      }
   }
   if (bits_collected > 0) { // Any trailing bits that are missing.
      assert(bits_collected < 6);
      accumulator <<= 6 - bits_collected;
      retval[outpos++] = b64_table[accumulator & 0x3fu];
   }
   assert(outpos >= (retval.size() - 2));
   assert(outpos <= retval.size());
   return retval;
}

std::string base64_decode(const std::string &ascdata)
{
   using std::string;
   string retval;
   const string::const_iterator last = ascdata.end();
   int bits_collected = 0;
   unsigned int accumulator = 0;

   for (string::const_iterator i = ascdata.begin(); i != last; ++i) {
      const int c = *i;
      if (std::isspace(c) || c == '=') {
         // Skip whitespace and padding. Be liberal in what you accept.
         continue;
      }
      if ((c > 127) || (c < 0) || (reverse_table[c] > 63)) {
         throw std::invalid_argument("This contains characters not legal in a base64 encoded string.");
      }
      accumulator = (accumulator << 6) | reverse_table[c];
      bits_collected += 6;
      if (bits_collected >= 8) {
         bits_collected -= 8;
         retval += (char)((accumulator >> bits_collected) & 0xffu);
      }
   }
   return retval;
}

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
        cout << base64_encode(std::string((char*)encrypt_text, rsa_len)) << endl;

        delete[] encrypt_text;
        fclose(fp);
    }
    delete[] text;
    return 0;
}

