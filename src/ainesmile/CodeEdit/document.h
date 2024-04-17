#pragma once

#include "encodingutils.h"

class ASDocument
{
public:
    [[nodiscard]] std::tuple<bool, QByteArray> loadFromFile();
    bool                                       saveToFile(const QByteArray &data);

    [[nodiscard]] std::tuple<QString, QString> errorMessage() const;

    void                  setFilePath(const QString &filePath);
    [[nodiscard]] QString filePath() const;

    [[nodiscard]] QString encoding() const;
    void                  setEncoding(const QString &encoding);

    void               setBOM(BOM bom);
    [[nodiscard]] BOM  bom() const;
    [[nodiscard]] bool hasBOM() const;

    void               setForceEncoding(bool forceEncoding);
    [[nodiscard]] bool forceEncoding() const;

private:
    bool                     m_forceEncoding {false};
    QString                  m_filePath;
    QString                  m_encoding {QStringLiteral("UTF-8")};
    QString                  m_errorMessage;
    BOM                      m_bom {BOM::None};
    [[nodiscard]] QByteArray convertDataEncoding(const QByteArray &data, const QString &fromEncoding, const QString &toEncoding);
};