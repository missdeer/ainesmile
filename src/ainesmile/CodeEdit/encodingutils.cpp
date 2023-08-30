#include "stdafx.h"

#include <boost/scope_exit.hpp>
#include <unicode/ucnv.h>
#include <unicode/ucsdet.h>
#include <unicode/utext.h>

#include "encodingutils.h"

namespace EncodingUtils
{
    std::pair<BOM, std::uint8_t> checkBOM(const QByteArray &data)
    {
        static std::map<QByteArray, BOM> bomMap = {
            {QByteArrayLiteral("\xEF\xBB\xBF"), BOM::UTF8},
            {QByteArrayLiteral("\xFF\xFE"), BOM::UTF16LE},
            {QByteArrayLiteral("\xFE\xFF"), BOM::UTF16BE},
            {QByteArrayLiteral("\xFF\xFE\x00\x00"), BOM::UTF32LE},
            {QByteArrayLiteral("\x00\x00\xFE\xFF"), BOM::UTF32BE},
            {QByteArrayLiteral("\x2B\x2F\x76"), BOM::UTF7},
            {QByteArrayLiteral("\xF7\x64\x4C"), BOM::UTF1},
            {QByteArrayLiteral("\xDD\x73\x66\x73"), BOM::UTFEBCDIC},
            {QByteArrayLiteral("\x0E\xFE\xFF"), BOM::SCSU},
            {QByteArrayLiteral("\xFB\xEE\x28"), BOM::BOCU1},
            {QByteArrayLiteral("\x84\x31\x95\x33"), BOM::GB18030},
        };
        for (auto &[bytes, bom] : bomMap)
        {
            if (data.length() >= bytes.length() && bytes == data.mid(0, bytes.length()))
            {
                return {bom, bytes.length()};
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

        ucsdet_setText(csd, data.constData(), data.length(), &status);
        if (U_FAILURE(status))
        {
            return QStringLiteral("UTF-8");
        }

        const UCharsetMatch *match = ucsdet_detect(csd, &status);
        if (U_FAILURE(status))
        {
            return QStringLiteral("UTF-8");
        }

        const char *charset = ucsdet_getName(match, &status);
        if (U_FAILURE(status))
        {
            return QStringLiteral("UTF-8");
        }

        return QString::fromLatin1(charset);
    }
} // namespace EncodingUtils