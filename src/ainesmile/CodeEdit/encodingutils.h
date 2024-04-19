#pragma once

#include <cstdint>

#include <QByteArray>
#include <QString>

enum class BOM : std::uint8_t
{
    None,
    UTF8,
    UTF16LE,
    UTF16BE,
    UTF32LE,
    UTF32BE,
    UTF7,
    UTF1,
    UTFEBCDIC,
    SCSU,
    BOCU1,
    GB18030,
};

namespace EncodingUtils
{
    QString                      fileEncodingDetect(const char *data, int length);
    QString                      fileEncodingDetect(const QByteArray &data);
    std::pair<BOM, std::uint8_t> checkBOM(const QByteArray &data);
    QByteArray                   encodingNameForBOM(BOM bom);
    QByteArray                   generateBOM(BOM bom);

} // namespace EncodingUtils
