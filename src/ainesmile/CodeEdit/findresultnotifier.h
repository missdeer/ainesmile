#pragma once

#include <QObject>

class FindResultNotifier : public QObject
{
    Q_OBJECT

public:
    explicit FindResultNotifier(QObject *parent = nullptr);

    void addResult(const QString &filePath, int startPos, int length);
signals:
    void found(QString, int, int);
};
