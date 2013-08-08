#ifndef STUPIDCHECK_H
#define STUPIDCHECK_H

#include <QString>
class StupidCheck
{
public:
    StupidCheck();
    void save(const QString& username, const QString& licenseCode);
    QString getPinCode();
    bool isValid();
};

#endif // STUPIDCHECK_H
