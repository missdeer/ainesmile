#include "stdafx.h"

#include <QFile>

#include "document.h"
#include "config.h"

namespace
{
    const qint64 fileMappingBlockSize = 4 * 1024 * 1024;
} // namespace

void ASDocument::closeConverters(UConverter *fromConverter, UConverter *toConverter)
{
    ucnv_close(fromConverter);
    ucnv_close(toConverter);
}

std::tuple<UConverter *, UConverter *> ASDocument::prepareConverters(const QString &fromEncoding, const QString &toEncoding)
{
    UErrorCode  errorCode = U_ZERO_ERROR;
    UConverter *fromConv  = ucnv_open(fromEncoding.toStdString().c_str(), &errorCode);
    if (U_FAILURE(errorCode))
    {
        const char *errorName = u_errorName(errorCode);
        m_errorMessage        = QObject::tr("creating converter for %1 failed: %2").arg(fromEncoding).arg(errorName);
        return {nullptr, nullptr};
    }
    UConverter *toConv = ucnv_open(toEncoding.toStdString().c_str(), &errorCode);
    if (U_FAILURE(errorCode))
    {
        const char *errorName = u_errorName(errorCode);
        m_errorMessage        = QObject::tr("creating converter for %1 failed: %2").arg(toEncoding).arg(errorName);
        ucnv_close(fromConv);
        return {nullptr, nullptr};
    }
    return {fromConv, toConv};
}

std::tuple<QByteArray, int> ASDocument::convertDataEncoding(const char *data, qint64 length, UConverter *sourceConv, UConverter *targetConv)
{
    UErrorCode errorCode = U_ZERO_ERROR;

    const qint64      sourceSize  = length;
    const char       *source      = data;
    const char *const sourceStart = data;
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
        m_errorMessage        = QObject::tr("converting failed: %3").arg(errorName);
        return {{data, static_cast<int>(length)}, static_cast<int>(length)};
    }
    ptrdiff_t bytesConsumed  = source - sourceStart;
    ptrdiff_t bytesGenerated = target - targetStart;
    targetBuffer.resize(bytesGenerated);
    return {targetBuffer, bytesConsumed};
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

    auto [fromConv, toConv] = prepareConverters(QStringLiteral("UTF-8"), m_encoding);
    if (!fromConv || !toConv)
    {
        m_errorMessage = QObject::tr("creating converter for %1 & UTF-8 failed").arg(m_encoding);
        return false;
    }
    auto [newData, _] = convertDataEncoding(data.constData(), data.length(), fromConv, toConv);
    closeConverters(fromConv, toConv);
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

bool ASDocument::loadEncodedFile(
    QFile &file, const QString &fromEncoding, const QString &toEncoding, std::function<void(qint64, const char *)> load, qint64 offset)
{
    auto [fromConv, toConv] = prepareConverters(fromEncoding, toEncoding);
    if (!fromConv || !toConv)
    {
        m_errorMessage = QObject::tr("creating converter for %1 & UTF-8 failed").arg(m_encoding);
        return false;
    }
    BOOST_SCOPE_EXIT(this_, fromConv, toConv)
    {
        this_->closeConverters(fromConv, toConv);
    }
    BOOST_SCOPE_EXIT_END
    qint64 leftSize = file.size() - offset;
    while (leftSize > 0)
    {
        qint64 expectedSize = std::min(leftSize, fileMappingBlockSize);
        auto  *mappedData   = file.map(offset, expectedSize);
        if (!mappedData)
        {
            m_errorMessage = QObject::tr("creating file mapping failed");
            return false;
        }
        auto [decodedData, bytesConsumed] = convertDataEncoding((const char *)mappedData, expectedSize, fromConv, toConv);
        load(decodedData.length(), decodedData.constData());
        offset += bytesConsumed;
        leftSize -= bytesConsumed;
        file.unmap(mappedData);
    }
    return true;
}

bool ASDocument::loadFile(QFile &file, std::function<void(qint64, const char *)> load, qint64 offset)
{
    qint64 leftSize = file.size() - offset;
    while (leftSize > 0)
    {
        qint64 expectedSize = std::min(leftSize, fileMappingBlockSize);
        auto  *mappedData   = file.map(offset, expectedSize);
        if (!mappedData)
        {
            m_errorMessage = QObject::tr("creating file mapping failed");
            return false;
        }
        offset += expectedSize;
        leftSize -= expectedSize;
        load(expectedSize, (const char *)mappedData);
        file.unmap(mappedData);
    }
    return true;
}

bool ASDocument::loadFromFile(std::function<void(qint64, const char *)> load)
{
    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        return false;
    }

    auto &ptree              = Config::instance()->pt();
    bool  autoDetectEncoding = ptree.get<bool>("encoding.auto_detect", false);

    // Fast path: no encoding detection needed
    if (!m_forceEncoding && !autoDetectEncoding)
    {
        m_encoding = QStringLiteral("UTF-8");
        m_bom      = BOM::None;
        return loadFile(file, load);
    }

    // User specified encoding
    if (m_forceEncoding)
    {
        m_bom = BOM::None;
        if (m_encoding.compare(QStringLiteral("UTF-8"), Qt::CaseInsensitive) == 0)
        {
            m_encoding = QStringLiteral("UTF-8");
            return loadFile(file, load);
        }
        return loadEncodedFile(file, m_encoding, QStringLiteral("UTF-8"), load);
    }

    // Auto-detect encoding
    qint64 fileSize = file.size();

    // Handle empty or invalid file size
    if (fileSize <= 0)
    {
        m_encoding = QStringLiteral("UTF-8");
        m_bom      = BOM::None;
        return loadFile(file, load);
    }

    // Map file once for both BOM check and ICU detection
    qint64 expectedSize = std::min(fileSize, fileMappingBlockSize);
    auto  *mappedData   = file.map(0, expectedSize);
    if (!mappedData)
    {
        m_errorMessage = QObject::tr("creating file mapping failed");
        return false;
    }

    // Check BOM first
    auto [bom, bomLength] = EncodingUtils::checkBOM(QByteArray::fromRawData(reinterpret_cast<const char *>(mappedData), expectedSize));

    if (bom != BOM::None)
    {
        m_bom              = bom;
        QByteArray encName = EncodingUtils::encodingNameForBOM(bom);

        // If BOM is recognized but encoding is not supported (UTF7, UTF1, etc.),
        // fall through to ICU detection
        if (!encName.isEmpty())
        {
            m_encoding = encName;
            file.unmap(mappedData);

            if (bom == BOM::UTF8)
            {
                return loadFile(file, load, bomLength);
            }
            return loadEncodedFile(file, m_encoding, QStringLiteral("UTF-8"), load, bomLength);
        }
        // Unsupported BOM - reset and try ICU detection
        m_bom = BOM::None;
    }

    // No BOM found (or unsupported BOM), use ICU to detect encoding
    QString encoding = EncodingUtils::fileEncodingDetect(reinterpret_cast<const char *>(mappedData), static_cast<int>(expectedSize));
    file.unmap(mappedData);

    if (!encoding.isEmpty() && encoding.compare(QStringLiteral("UTF-8"), Qt::CaseInsensitive) != 0)
    {
        m_encoding = encoding;
        return loadEncodedFile(file, m_encoding, QStringLiteral("UTF-8"), load);
    }

    m_encoding = QStringLiteral("UTF-8");
    return loadFile(file, load);
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