#include "stdafx.h"

#include <array>

#include <QTextCodec>

#include "encodingutils.h"

namespace EncodingUtils
{
    std::pair<BOM, std::uint8_t> checkBOM(const QByteArray &data)
    {
        // Ordered by length descending to avoid misdetection
        // (e.g., UTF-32LE starts with same bytes as UTF-16LE)
        static const std::array bomList = {
            std::pair {QByteArrayLiteral("\xFF\xFE\x00\x00"), BOM::UTF32LE},
            std::pair {QByteArrayLiteral("\x00\x00\xFE\xFF"), BOM::UTF32BE},
            std::pair {QByteArrayLiteral("\x84\x31\x95\x33"), BOM::GB18030},
            std::pair {QByteArrayLiteral("\xDD\x73\x66\x73"), BOM::UTFEBCDIC},
            std::pair {QByteArrayLiteral("\xEF\xBB\xBF"), BOM::UTF8},
            std::pair {QByteArrayLiteral("\x2B\x2F\x76"), BOM::UTF7},
            std::pair {QByteArrayLiteral("\xF7\x64\x4C"), BOM::UTF1},
            std::pair {QByteArrayLiteral("\x0E\xFE\xFF"), BOM::SCSU},
            std::pair {QByteArrayLiteral("\xFB\xEE\x28"), BOM::BOCU1},
            std::pair {QByteArrayLiteral("\xFF\xFE"), BOM::UTF16LE},
            std::pair {QByteArrayLiteral("\xFE\xFF"), BOM::UTF16BE},
        };
        for (const auto &[bytes, bom] : bomList)
        {
            if (data.length() >= bytes.length() && data.startsWith(bytes))
            {
                return {bom, static_cast<std::uint8_t>(bytes.length())};
            }
        }
        return {BOM::None, 0};
    }

    QByteArray encodingNameForBOM(BOM bom)
    {
        static std::map<BOM, QByteArray> encodingNameMap = {
            {BOM::UTF8, QByteArrayLiteral("UTF-8")},
            {BOM::UTF16LE, QByteArrayLiteral("UTF-16LE")},
            {BOM::UTF16BE, QByteArrayLiteral("UTF-16BE")},
            {BOM::UTF32LE, QByteArrayLiteral("UTF-32LE")},
            {BOM::UTF32BE, QByteArrayLiteral("UTF-32BE")},
            {BOM::GB18030, QByteArrayLiteral("GB18030")},
        };
        auto iter = encodingNameMap.find(bom);
        if (encodingNameMap.end() != iter)
        {
            return iter->second;
        }

        return {};
    }

    QByteArray generateBOM(BOM bom)
    {
        static std::map<BOM, QByteArray> codecNameMap = {
            {BOM::UTF8, QByteArrayLiteral("\xEF\xBB\xBF")},
            {BOM::UTF16LE, QByteArrayLiteral("\xFF\xFE")},
            {BOM::UTF16BE, QByteArrayLiteral("\xFE\xFF")},
            {BOM::UTF32LE, QByteArrayLiteral("\xFF\xFE\x00\x00")},
            {BOM::UTF32BE, QByteArrayLiteral("\x00\x00\xFE\xFF")},
            {BOM::GB18030, QByteArrayLiteral("\x84\x31\x95\x33")},
        };
        auto iter = codecNameMap.find(bom);
        if (codecNameMap.end() != iter)
        {
            return iter->second;
        }
        return {};
    }

    QString fileEncodingDetect(const QByteArray &data)
    {
        return fileEncodingDetect(data.constData(), data.length());
    }

    QString fileEncodingDetect(const char *data, int length)
    {
        UErrorCode status = U_ZERO_ERROR;

        UCharsetDetector *csd = ucsdet_open(&status);
        if (U_FAILURE(status))
        {
            return QStringLiteral("UTF-8");
        }
        BOOST_SCOPE_EXIT(csd)
        {
            ucsdet_close(csd);
        }
        BOOST_SCOPE_EXIT_END

        ucsdet_setText(csd, data, length, &status);
        if (U_FAILURE(status))
        {
            return QStringLiteral("UTF-8");
        }
        int32_t               matchesFound = 0;
        const UCharsetMatch **matches      = ucsdet_detectAll(csd, &matchesFound, &status);
        if (U_FAILURE(status) || matchesFound == 0)
        {
            return QStringLiteral("UTF-8");
        }

        using encodingPair = std::pair<QString, int32_t>;
        std::vector<encodingPair> encodings;
        for (int32_t i = 0; i < matchesFound; i++)
        {
            UErrorCode matchStatus = U_ZERO_ERROR;
            auto       confidence  = ucsdet_getConfidence(matches[i], &matchStatus);
            if (U_FAILURE(matchStatus) || confidence < 50)
            {
                continue;
            }
            matchStatus      = U_ZERO_ERROR;
            const char *name = ucsdet_getName(matches[i], &matchStatus);
            if (U_FAILURE(matchStatus))
            {
                continue;
            }
            encodings.emplace_back(QString::fromLatin1(name), confidence);
        }

        if (!encodings.empty())
        {
            return encodings.front().first;
        }
        QTextCodec *codec = QTextCodec::codecForLocale();
        if (!codec)
        {
            return QStringLiteral("UTF-8");
        }
        auto encoding = QString::fromLatin1(codec->name());
        if (encoding.isEmpty() || encoding.toLower() == QStringLiteral("system"))
        {
#if defined(Q_OS_WIN)
            const size_t bufferSize             = 100;
            WCHAR        localeData[bufferSize] = {0}; // Buffer to store locale information

            if (GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_IDEFAULTANSICODEPAGE, localeData, bufferSize))
            {
                return QStringLiteral("windows-") + QString::fromWCharArray(localeData);
            }
#endif
        }
        else
        {
            return encoding;
        }
        return QStringLiteral("UTF-8");
    }
} // namespace EncodingUtils
