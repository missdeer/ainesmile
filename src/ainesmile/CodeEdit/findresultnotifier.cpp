#include "stdafx.h"

#include "findresultnotifier.h"

FindResultNotifier::FindResultNotifier(QObject *parent) : QObject(parent) {}

void FindResultNotifier::addResult(const QString &filePath, int startPos, int length)
{
    emit found(filePath, startPos, length);
}
