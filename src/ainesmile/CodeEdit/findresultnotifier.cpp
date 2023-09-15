#include "stdafx.h"

#include "findresultnotifier.h"

FindResultNotifier::FindResultNotifier(QObject *parent) : QObject(parent) {}

void FindResultNotifier::addResult(const QString &filePath, const QString &context, int line, int column, int length)
{
    emit found(filePath, context, line, column, length);
}
