#include "stdafx.h"

#include <QFile>

#include "document.h"
#include "config.h"

QByteArray ASDocument::convertReadDataEncoding(const QByteArray &data)
{
    UErrorCode errorCode = U_ZERO_ERROR;

    UConverter *sourceConv = ucnv_open(m_encoding.toStdString().c_str(), &errorCode);
    if (U_FAILURE(errorCode))
    {
        const char *errorName = u_errorName(errorCode);
        m_errorMessage        = QObject::tr("creating converter for %1 failed: %2").arg(m_encoding).arg(errorName);
        return {};
    }
    BOOST_SCOPE_EXIT(sourceConv)
    {
        ucnv_close(sourceConv);
    }
    BOOST_SCOPE_EXIT_END

    UConverter *targetConv = ucnv_open("UTF-8", &errorCode);
    if (U_FAILURE(errorCode))
    {
        const char *errorName = u_errorName(errorCode);
        m_errorMessage        = QObject::tr("creating converter for UTF-8 failed: %1").arg(errorName);
        return {};
    }
    BOOST_SCOPE_EXIT(targetConv)
    {
        ucnv_close(targetConv);
    }
    BOOST_SCOPE_EXIT_END

    const qint64 sourceSize  = data.length();
    const char  *source      = data.constData();
    const char  *sourceStart = data.constData();
    const char  *sourceLimit = source + sourceSize;

    const qint64 targetBufferSize = sourceSize * 2;
    QByteArray   targetBuffer;
    targetBuffer.resize(targetBufferSize);
    char *target      = targetBuffer.data();
    char *targetStart = target;
    char *targetLimit = target + targetBufferSize;
    // convert encoding
    ucnv_convertEx(targetConv, sourceConv, &target, targetLimit, &source, sourceLimit, nullptr, nullptr, nullptr, nullptr, true, true, &errorCode);

    if (errorCode == U_BUFFER_OVERFLOW_ERROR)
    {
        errorCode = U_ZERO_ERROR;
    }
    else if (U_FAILURE(errorCode))
    {
        const char *errorName = u_errorName(errorCode);
        m_errorMessage        = QObject::tr("converting from %1 to UTF-8 failed: %2").arg(m_encoding).arg(errorName);
        return {};
    }
    ptrdiff_t bytesConsumed  = source - sourceStart;
    ptrdiff_t bytesGenerated = target - targetStart;
    targetBuffer.resize(bytesGenerated);
    return targetBuffer;
}

bool ASDocument::saveToFile(const QByteArray &data)
{
    QFile file(m_filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        m_errorMessage = QObject::tr("Cannot write to file %1.").arg(QDir::toNativeSeparators(m_filePath));
        return false;
    }

    BOOST_SCOPE_EXIT(&file)
    {
        file.close();
    }
    BOOST_SCOPE_EXIT_END

    //  check bom & encoding
    auto bytes = EncodingUtils::generateBOM(m_bom);
    if (!bytes.isEmpty())
    {
        file.write(bytes);
    }
    if (m_encoding.toUpper() == QByteArrayLiteral("UTF-8") || m_encoding.toLower() == QByteArrayLiteral("ASCII") ||
        m_encoding.toLower() == QByteArrayLiteral("ANSI"))
    {
        file.write(data);
        return true;
    }

    auto newData = convertWriteDataEncoding(data);
    file.write(newData);
    return true;
}

void ASDocument::setFilePath(const QString &filePath)
{
    m_filePath = filePath;
}

std::tuple<QString, QString> ASDocument::errorMessage() const
{
    return std::make_tuple(QObject::tr("Error"), m_errorMessage);
}
QString ASDocument::filePath() const
{
    return m_filePath;
}
BOM ASDocument::bom() const
{
    return m_bom;
}
QString ASDocument::encoding() const
{
    return m_encoding;
}
bool ASDocument::hasBOM() const
{
    return m_bom != BOM::None;
}

QByteArray ASDocument::loadFromFile()
{
    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        return {};
    }

    if (m_forceEncoding)
    {
        auto data = file.readAll();
        return convertReadDataEncoding(data);
    }

    auto &ptree              = Config::instance()->pt();
    bool  autoDetectEncoding = ptree.get<bool>("encoding.auto_detect", false);

    m_bom                                 = BOM::None;
    const qint64                headerLen = 4;
    std::array<char, headerLen> header {};
    qint64                      cbRead          = file.read(header.data(), headerLen);
    bool                        charsetDetected = false;
    auto [bom, length]                          = EncodingUtils::checkBOM(QByteArray::fromRawData(header.data(), cbRead));

    file.seek(length);
    auto data = file.readAll();
    if (bom == BOM::UTF8)
    {
        m_encoding = QByteArrayLiteral("UTF-8");
        m_bom      = bom;
        return data;
    }
    if (bom != BOM::None)
    {
        auto encoding   = EncodingUtils::encodingNameForBOM(bom);
        m_encoding      = encoding;
        m_bom           = bom;
        charsetDetected = true;
        return convertReadDataEncoding(data);
    }

    if (autoDetectEncoding)
    {
        if (!charsetDetected)
        {
            m_bom = BOM::None;
            file.seek(0);
            QString encoding = EncodingUtils::fileEncodingDetect(data);
            if (!encoding.isEmpty() && encoding.toUpper() != QByteArrayLiteral("UTF-8"))
            {
                m_encoding      = encoding.toUtf8();
                charsetDetected = true;
                return convertReadDataEncoding(data);
            }
        }
    }
    if (!charsetDetected)
    {
        m_encoding = QByteArrayLiteral("UTF-8");
        return data;
    }
    return {};
}

void ASDocument::setEncoding(const QString &encoding)
{
    m_encoding = encoding;
}

void ASDocument::setBOM(BOM bom)
{
    m_bom = bom;
}

QByteArray ASDocument::convertWriteDataEncoding(const QByteArray &data)
{
    UErrorCode errorCode = U_ZERO_ERROR;

    UConverter *sourceConv = ucnv_open("UTF-8", &errorCode);
    if (U_FAILURE(errorCode))
    {
        const char *errorName = u_errorName(errorCode);
        m_errorMessage        = QObject::tr("creating converter for UTF-8 failed: %1").arg(errorName);
        return data;
    }
    BOOST_SCOPE_EXIT(sourceConv)
    {
        ucnv_close(sourceConv);
    }
    BOOST_SCOPE_EXIT_END

    UConverter *targetConv = ucnv_open(m_encoding.toStdString().c_str(), &errorCode);
    if (U_FAILURE(errorCode))
    {
        const char *errorName = u_errorName(errorCode);
        m_errorMessage        = QObject::tr("creating converter for %1 failed: %2").arg(m_encoding).arg(errorName);
        return data;
    }
    BOOST_SCOPE_EXIT(targetConv)
    {
        ucnv_close(targetConv);
    }
    BOOST_SCOPE_EXIT_END
    qint64      textLength  = data.size();
    const char *source      = data.constData();
    const char *sourceStart = source;
    const char *sourceLimit = source + textLength;

    const qint64 targetBufferSize = textLength * 2;
    QByteArray   targetBuffer;
    targetBuffer.resize(targetBufferSize);
    char *target      = targetBuffer.data();
    char *targetStart = target;
    char *targetLimit = target + targetBufferSize;
    // convert encoding
    ucnv_convertEx(targetConv, sourceConv, &target, targetLimit, &source, sourceLimit, nullptr, nullptr, nullptr, nullptr, true, true, &errorCode);

    if (errorCode == U_BUFFER_OVERFLOW_ERROR)
    {
        errorCode = U_ZERO_ERROR;
    }
    else if (U_FAILURE(errorCode))
    {
        // Handle conversion error
        const char *errorName = u_errorName(errorCode);
        m_errorMessage        = QObject::tr("converting from UTF-8 to %1 failed: %2").arg(m_encoding, errorName);
        return data;
    }
    ptrdiff_t bytesGenerated = target - targetStart;
    targetBuffer.resize(bytesGenerated);
    return targetBuffer;
}

void ASDocument::setForceEncoding(bool forceEncoding)
{
    m_forceEncoding = forceEncoding;
}

bool ASDocument::forceEncoding() const
{
    return m_forceEncoding;
}