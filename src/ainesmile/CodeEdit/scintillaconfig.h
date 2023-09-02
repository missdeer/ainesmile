#ifndef SCINTILLACONFIG_H
#define SCINTILLACONFIG_H

class ScintillaEdit;

namespace ScintillaConfig
{
    void initScintilla(ScintillaEdit *sci);
    void initFolderStyle(ScintillaEdit *sci);
    void initEditorStyle(ScintillaEdit *sci, const QString &lang);

    void applyLanguageStyle(ScintillaEdit *sci, const QString &configPath);
    void applyThemeStyle(ScintillaEdit *sci, const QString &themePath);
} // namespace ScintillaConfig

#endif // SCINTILLACONFIG_H
