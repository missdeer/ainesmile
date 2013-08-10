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
    BIO_free(bufio);

    if (rsa)
    {
        int len = RSA_size(rsa) + 1;
        unsigned char *sz = (unsigned char*)calloc(len, sizeof(unsigned char));
        memset(sz, 0, len);
        QString pinCode = getPinCode();
        QString username;
        QString licenseCode;
        Config::instance()->loadLicenseInfo(username, licenseCode);
        RSA_public_decrypt(licenseCode.size(), (const unsigned char *)licenseCode.toStdString().c_str(), sz, rsa, RSA_PKCS1_PADDING);
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
