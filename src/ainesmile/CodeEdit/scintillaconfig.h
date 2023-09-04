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

    constexpr int smartHighlightIndicator()
    {
        return 1;
    }

    constexpr int bookmarkMarker()
    {
        return 24;
    }

    constexpr int bookmarkMargin()
    {
        return 1;
    }

    constexpr int bookmarkMask()
    {
        return 1 << bookmarkMarker();
    }
} // namespace ScintillaConfig

#endif // SCINTILLACONFIG_H
