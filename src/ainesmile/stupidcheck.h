#ifndef STUPIDCHECK_H
#define STUPIDCHECK_H

#include <QString>
class StupidCheck
{
private:
    bool isValid(const QString& publicPEM) const;
public:
    StupidCheck();
    void save(const QString& username, const QString& licenseCode);
    QString getPinCode() const;
    bool isStandard() const;
    bool isProfessional() const;
    bool isDeluxe() const;
};

#endif // STUPIDCHECK_H
