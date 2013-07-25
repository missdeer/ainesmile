#ifndef SCINTILLACONFIG_H
#define SCINTILLACONFIG_H

class ScintillaEdit;

class ScintillaConfig
{
public:
    ScintillaConfig();
    void initScintilla(ScintillaEdit* sci);
    void initFolderStyle(ScintillaEdit* sci);
    void initEditorStyle(ScintillaEdit* sci);
};

#endif // SCINTILLACONFIG_H
