#include <QFile>
#include <QDesktopServices>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QStandardPaths>
#endif
#include <QLineEdit>
#include <QMessageBox>
#include <cstdio>
#include <iomanip>
#include <sstream>
#include <string>
#include <cassert>
#include <limits>
#include <stdexcept>
#include <cctype>
#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/bn.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include "util.hpp"
#include "config.h"
#include "stupidcheck.h"

static const char b64_table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const char reverse_table[128] =
{
   64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
   64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
   64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
   52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
   64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
   15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
   64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
   41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64
};

StupidCheck::StupidCheck()
{
}

void StupidCheck::save(const QString &username, const QString &licenseCode)
{
    Config* cfg = Config::instance();
    cfg->saveLicenseInfo(username, licenseCode);
}

QString StupidCheck::getPinCode() const
{
    uint64 peerId = utility::utilities::get_local_peer_id();
    Q_ASSERT(peerId);
    std::stringstream ss;
    ss << std::setw(16) << std::setfill('0') << std::hex << peerId;

    return QString::fromStdString(ss.str());
}

bool StupidCheck::isStandard() const
{
    return isValid(":/public-standard.pem");
}

bool StupidCheck::isProfessional() const
{
    return isValid(":/public-professional.pem");
}

bool StupidCheck::isDeluxe() const
{
    return isValid(":/public-deluxe.pem");
}

bool StupidCheck::isValid(const QString& publicPEM) const
{
    QFile resFile(publicPEM);
    resFile.open(QFile::ReadOnly | QFile::Text);
    QByteArray pubKey = resFile.readAll();
    resFile.close();

    RSA* rsa = NULL;
    BIO *bufio = BIO_new_mem_buf((void*)pubKey.data(), pubKey.size());
    PEM_read_bio_RSA_PUBKEY(bufio, &rsa, 0, NULL);
    BIO_free_all(bufio);

    if (rsa)
    {
        int len = RSA_size(rsa) + 1;
        unsigned char *sz = (unsigned char*)calloc(len, sizeof(unsigned char));
        memset(sz, 0, len);
        QString pinCode = getPinCode();
        QString username;
        QString licenseCode;
        Config::instance()->loadLicenseInfo(username, licenseCode);
        std::string realLicenseCode = base64_decode(licenseCode.toStdString());
        RSA_public_decrypt(realLicenseCode.size(), (const unsigned char *)realLicenseCode.c_str(), sz, rsa, RSA_PKCS1_PADDING);
        QString original;
        for (int i = 0; i < (username.size() > pinCode.size() ? username.size() : pinCode.size()); i++)
        {
            if (i < username.size())
                original.append(username.at(i));
            if (i < pinCode.size())
                original.append(pinCode.at(i));
        }

        if (original == (const char*)sz)
        {
            return true;
        }
    }

    return false;

}

std::string StupidCheck::base64_encode(const std::string &bindata) const
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

std::string StupidCheck::base64_decode(const std::string &ascdata) const
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
