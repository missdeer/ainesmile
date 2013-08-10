#ifndef STUPIDCHECK_H
#define STUPIDCHECK_H

#include <string>
#include <QString>

class StupidCheck
{
private:
    bool isValid(const QString& publicPEM) const;

    std::string base64_encode(const std::string &bindata) const;

    std::string base64_decode(const std::string &ascdata) const;
public:
    StupidCheck();
    void save(const QString& username, const QString& licenseCode);
    QString getPinCode() const;
    bool isStandard() const;
    bool isProfessional() const;
    bool isDeluxe() const;
};

#endif // STUPIDCHECK_H
