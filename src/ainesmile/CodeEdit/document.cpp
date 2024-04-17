#include "stdafx.h"

#include <QFile>

#include "document.h"
#include "config.h"

QByteArray ASDocument::convertDataEncoding(const QByteArray &data, const QString &fromEncoding, const QString &toEncoding)
{
    UErrorCode errorCode = U_ZERO_ERROR;

    UConverter *sourceConv = ucnv_open(fromEncoding.toStdString().c_str(), &errorCode);
    if (U_FAILURE(errorCode))
    {
        const char *errorName = u_errorName(errorCode);
        m_errorMessage        = QObject::tr("creating converter for %1 failed: %2").arg(fromEncoding).arg(errorName);
        return data;
    }

    UConverter *targetConv = ucnv_open(toEncoding.toStdString().c_str(), &errorCode);
    if (U_FAILURE(errorCode))
    {
        const char *errorName = u_errorName(errorCode);
        m_errorMessage        = QObject::tr("creating converter for UTF-8 failed: %1").arg(errorName);
        ucnv_close(sourceConv);
        return data;
    }
    BOOST_SCOPE_EXIT(sourceConv, targetConv)
    {
        ucnv_close(sourceConv);
        ucnv_close(targetConv);
    }
    BOOST_SCOPE_EXIT_END

    const qint64      sourceSize  = data.length();
    const char       *source      = data.constData();
    const char *const sourceStart = data.constData();
    const char *const sourceLimit = source + sourceSize;

    const qint64 targetBufferSize = sourceSize * 4;
    QByteArray   targetBuffer;
    targetBuffer.resize(targetBufferSize);
    char             *target      = targetBuffer.data();
    const char *const targetStart = target;
    char             *targetLimit = target + targetBufferSize;
    // convert encoding
    ucnv_convertEx(targetConv, sourceConv, &target, targetLimit, &source, sourceLimit, nullptr, nullptr, nullptr, nullptr, true, true, &errorCode);

    if (errorCode == U_BUFFER_OVERFLOW_ERROR)
    {
        qint64 usedSize       = target - targetStart;
        qint64 additionalSize = targetBufferSize * 2 - usedSize;
        targetBuffer.resize(usedSize + additionalSize);
        target      = targetBuffer.data() + usedSize;
        targetLimit = targetBuffer.data() + targetBuffer.size();
        errorCode   = U_ZERO_ERROR;
        // retry
        ucnv_convertEx(
            targetConv, sourceConv, &target, targetLimit, &source, sourceLimit, nullptr, nullptr, nullptr, nullptr, true, true, &errorCode);
    }

    if (U_FAILURE(errorCode))
    {
        // Handle conversion error
        const char *errorName = u_errorName(errorCode);
        m_errorMessage        = QObject::tr("converting from %1 to %2 failed: %3").arg(fromEncoding).arg(toEncoding).arg(errorName);
        return data;
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

    auto newData = convertDataEncoding(data, QStringLiteral("UTF-8"), m_encoding);
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

std::tuple<bool, QByteArray> ASDocument::loadFromFile()
{
    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        return {false, {}};
    }

    if (m_forceEncoding)
    {
        auto data = file.readAll();
        return {true, convertDataEncoding(data, m_encoding, QStringLiteral("UTF-8"))};
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
        return {true, data};
    }
    if (bom != BOM::None)
    {
        auto encoding   = EncodingUtils::encodingNameForBOM(bom);
        m_encoding      = encoding;
        m_bom           = bom;
        charsetDetected = true;
        return {true, convertDataEncoding(data, m_encoding, QStringLiteral("UTF-8"))};
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
                return {true, convertDataEncoding(data, m_encoding, QStringLiteral("UTF-8"))};
            }
        }
    }
    if (!charsetDetected)
    {
        m_encoding = QByteArrayLiteral("UTF-8");
        return {true, data};
    }
    return {false, {}};
}

void ASDocument::setEncoding(const QString &encoding)
{
    m_encoding = encoding;
}

void ASDocument::setBOM(BOM bom)
{
    m_bom = bom;
}

void ASDocument::setForceEncoding(bool forceEncoding)
{
    m_forceEncoding = forceEncoding;
}

bool ASDocument::forceEncoding() const
{
    return m_forceEncoding;
}