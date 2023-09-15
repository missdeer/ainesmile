#pragma once

#include <QObject>

class FindResultNotifier : public QObject
{
    Q_OBJECT

public:
    explicit FindResultNotifier(QObject *parent = nullptr);

    void addResult(const QString &filePath, const QString &context, int line, int column, int length);
signals:
    void found(QString, QString, int, int, int);
};
