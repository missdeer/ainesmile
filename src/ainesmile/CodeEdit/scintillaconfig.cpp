#include "ScintillaEdit.h"
#include "scintillaconfig.h"

ScintillaConfig::ScintillaConfig()
{
}

void ScintillaConfig::initScintilla(ScintillaEdit* sci)
{
    sci->styleResetDefault();
    sci->styleClearAll();
    sci->clearDocumentStyle();
    sci->setHScrollBar(true);
    sci->setVScrollBar(true);
    sci->setXOffset(0);
    sci->setScrollWidth(1);
    sci->setScrollWidthTracking(true);
    sci->setEndAtLastLine(true);
    sci->setWhitespaceFore(true, 0x808080);
    sci->setWhitespaceBack(true, 0xFFFFFF);
    sci->setMouseDownCaptures(true);
    sci->setEOLMode(SC_EOL_LF);
    sci->setViewEOL(false);
    sci->setStyleBits(5);
    sci->setCaretFore(0x0000FF);
    sci->setCaretLineVisible(true);
    sci->setCaretLineBack(0xFFFFD0);
    sci->setCaretLineBackAlpha(256);
    sci->setCaretPeriod(500);
    sci->setCaretWidth(2);
    sci->setCaretSticky(0);
    sci->setSelFore(true, 0xFFFFFF);
    sci->setSelBack(true, 0xC56A31);
    sci->setSelAlpha(256);
    sci->setSelEOLFilled(true);
    sci->setAdditionalSelectionTyping(true);
    sci->setVirtualSpaceOptions(SCVS_RECTANGULARSELECTION);
    sci->setHotspotActiveFore(true, 0x0000FF);
    sci->setHotspotActiveBack(true, 0xFFFFFF);
    sci->setHotspotActiveUnderline(true);
    sci->setHotspotSingleLine(false);
    sci->setControlCharSymbol(0);
    sci->setMarginLeft(3);
    sci->setMarginRight(3);
    sci->setMarginTypeN(0, SC_MARGIN_NUMBER);
    sci->setMarginWidthN(0, 24);
    sci->setMarginMaskN(0, 0);
    sci->setMarginSensitiveN(0, false);
    sci->setMarginTypeN(1, SC_MARGIN_SYMBOL);
    sci->setMarginWidthN(1, 14);
    sci->setMarginMaskN(1, 33554431);
    sci->setMarginSensitiveN(1, true);
    sci->setMarginTypeN(2, SC_MARGIN_SYMBOL);
    sci->setMarginWidthN(2, 14);
    sci->setMarginMaskN(2, SC_MASK_FOLDERS);// -33554432)
    sci->setMarginSensitiveN(2, true);

    sci->setFoldMarginColour(true, 0xCDCDCD);
    sci->setFoldMarginHiColour(true, 0xFFFFFF);

    sci->setTabWidth(4);
    sci->setUseTabs(false);
    sci->setIndent(4);
    sci->setTabIndents(false);
    sci->setBackSpaceUnIndents(false);
    sci->setIndentationGuides(0);
    sci->setHighlightGuide(1);
    sci->setPrintMagnification(1);
    sci->setPrintColourMode(0);
    sci->setPrintWrapMode(1);

    initFolderStyle( sci );

    sci->setWrapMode(SC_WRAP_NONE);
    sci->setWrapVisualFlags(SC_WRAPVISUALFLAG_NONE);
    sci->setWrapVisualFlagsLocation(SC_WRAPVISUALFLAGLOC_DEFAULT);
    sci->setWrapStartIndent(0);

    sci->setLayoutCache(2);
    sci->linesSplit(0);
    sci->setEdgeMode(0);
    sci->setEdgeColumn(200);
    sci->setEdgeColour(0xC0DCC0);

    sci->usePopUp(false);
    //sci->send(SCI_SETUSEPALETTE, 1, 0);
    sci->setBufferedDraw(true);
    sci->setTwoPhaseDraw(true);
    sci->setCodePage(SC_CP_UTF8);
    sci->setWordChars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_");
    sci->setZoom(1);
    sci->setWhitespaceChars(NULL);
    sci->setMouseDwellTime(2500);

    sci->setSavePoint();
    sci->setFontQuality(SC_EFF_QUALITY_LCD_OPTIMIZED);

    //sci:SetEncoding( cfg:GetString("config/encoding") )
}

void ScintillaConfig::initFolderStyle(ScintillaEdit *sci)
{
    sci->setFoldFlags(SC_FOLDFLAG_LINEAFTER_CONTRACTED);

    sci->markerDefine(SC_MARKNUM_FOLDEROPEN, SC_MARK_BOXMINUS);
    sci->markerDefine(SC_MARKNUM_FOLDER, SC_MARK_BOXPLUS);
    sci->markerDefine(SC_MARKNUM_FOLDERSUB, SC_MARK_VLINE);
    sci->markerDefine(SC_MARKNUM_FOLDERTAIL, SC_MARK_LCORNER);
    sci->markerDefine(SC_MARKNUM_FOLDEREND, SC_MARK_BOXPLUSCONNECTED);
    sci->markerDefine(SC_MARKNUM_FOLDEROPENMID, SC_MARK_BOXMINUSCONNECTED);
    sci->markerDefine(SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_TCORNER);

    sci->markerSetFore(SC_MARKNUM_FOLDEROPEN, 0xFFFFFF);
    sci->markerSetBack(SC_MARKNUM_FOLDEROPEN, 0x808080);
    sci->markerSetFore(SC_MARKNUM_FOLDER, 0xFFFFFF);
    sci->markerSetBack(SC_MARKNUM_FOLDER, 0x808080);
    sci->markerSetFore(SC_MARKNUM_FOLDERSUB, 0xFFFFFF);
    sci->markerSetBack(SC_MARKNUM_FOLDERSUB, 0x808080);
    sci->markerSetFore(SC_MARKNUM_FOLDERTAIL, 0xFFFFFF);
    sci->markerSetBack(SC_MARKNUM_FOLDERTAIL, 0x808080);
    sci->markerSetFore(SC_MARKNUM_FOLDEREND, 0xFFFFFF);
    sci->markerSetBack(SC_MARKNUM_FOLDEREND, 0x808080);
    sci->markerSetFore(SC_MARKNUM_FOLDEROPENMID, 0xFFFFFF);
    sci->markerSetBack(SC_MARKNUM_FOLDEROPENMID, 0x808080);
    sci->markerSetFore(SC_MARKNUM_FOLDERMIDTAIL, 0xFFFFFF);
    sci->markerSetBack(SC_MARKNUM_FOLDERMIDTAIL, 0x808080);
    sci->setProperty( "fold", "1");
    sci->setProperty( "fold.compact", "1");
    sci->setProperty( "fold.at.else", "1");
    sci->setProperty( "fold.preprocessor", "1");
    sci->setProperty( "fold.view", "1");
    sci->setProperty( "fold.comment", "1");
    sci->setProperty( "fold.html", "1");
    sci->setProperty( "fold.comment.python", "1");
    sci->setProperty( "fold.quotes.python", "1");
}

void ScintillaConfig::initEditorStyle(ScintillaEdit *sci)
{

}
