#pragma once

#include "encodingutils.h"

struct UConverter;
class ASDocument
{
public:
    bool loadFromFile(std::function<void(qint64, const char *)> load);
    bool saveToFile(const QByteArray &data);

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
    bool                                                 m_forceEncoding {false};
    QString                                              m_filePath;
    QString                                              m_encoding {QStringLiteral("UTF-8")};
    QString                                              m_errorMessage;
    BOM                                                  m_bom {BOM::None};
    [[nodiscard]] std::tuple<QByteArray, int>            convertDataEncoding(const char *data,
                                                                             qint64      length,
                                                                             UConverter *fromConverter,
                                                                             UConverter *toConverter);
    [[nodiscard]] std::tuple<UConverter *, UConverter *> prepareConverters(const QString &fromEncoding, const QString &toEncoding);
    void                                                 closeConverters(UConverter *fromConverter, UConverter *toConverter);
    bool                                                 loadFile(QFile &file, std::function<void(qint64, const char *)> load, qint64 offset = 0);
    bool                                                 loadEncodedFile(
                                                        QFile &file, const QString &fromEncoding, const QString &toEncoding, std::function<void(qint64, const char *)> load, qint64 offset = 0);
};