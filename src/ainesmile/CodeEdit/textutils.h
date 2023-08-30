#pragma once

#include <QByteArray>

namespace TextUtils
{
    int getLineCount(const char *pData, qint64 length);
    int getLineCount(const QByteArray &data);
} // namespace TextUtils