#pragma once

#include "encodingutils.h"

class ASDocument
{
public:
    [[nodiscard]] QByteArray loadFromFile();
    bool                     saveToFile(const QByteArray &data);

    [[nodiscard]] std::tuple<QString, QString> errorMessage() const;

    void                  setFilePath(const QString &filePath);
    [[nodiscard]] QString filePath() const;

    [[nodiscard]] QString encoding() const;
    void                  setEncoding(const QString &encoding);

    void               setBOM(BOM bom);
    [[nodiscard]] BOM  bom() const;
    [[nodiscard]] bool hasBOM() const;

private:
    QString                  m_filePath;
    QString                  m_encoding {QStringLiteral("UTF-8")};
    QString                  m_errorMessage;
    BOM                      m_bom {BOM::None};
    [[nodiscard]] QByteArray convertReadDataEncoding(const QByteArray &data);
    [[nodiscard]] QByteArray convertWriteDataEncoding(const QByteArray &data);
};