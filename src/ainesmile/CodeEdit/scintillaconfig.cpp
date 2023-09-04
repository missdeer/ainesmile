#include "stdafx.h"

#include <boost/scope_exit.hpp>

#include "scintillaconfig.h"
#include "ILexer.h"
#include "Lexilla.h"
#include "ScintillaEdit.h"
#include "config.h"

namespace ScintillaConfig
{
    void initScintilla(ScintillaEdit *sci)
    {
        Q_ASSERT(sci);
        boost::property_tree::ptree &ptree = Config::instance()->pt();

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
#if defined(Q_OS_WIN)
        sci->setEOLMode(SC_EOL_CRLF);
#else
        sci->setEOLMode(SC_EOL_LF);
#endif
        sci->setViewEOL(ptree.get<bool>("show.end_of_line", false));
        sci->setViewWS(ptree.get<bool>("show.white_space_and_tab", false) ? SCWS_VISIBLEALWAYS : SCWS_INVISIBLE);
        //    sci->setStyleBits(5);
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
        sci->setMarginLeft(4);
        // sci->setMarginRight(4);
        sci->setMarginTypeN(0, SC_MARGIN_NUMBER);
        sci->setMarginWidthN(0, 24);
        sci->setMarginMaskN(0, 0);
        sci->setMarginSensitiveN(0, false);
        sci->setMarginTypeN(1, SC_MARGIN_SYMBOL);
        sci->setMarginWidthN(1, 16);
        sci->setMarginMaskN(1, 33554431); //~SC_MASK_FOLDERS or 0x1FFFFFF or 33554431
        sci->setMarginSensitiveN(1, true);
        sci->setMarginTypeN(2, SC_MARGIN_SYMBOL);
        sci->setMarginWidthN(2, 16);
        sci->setMarginMaskN(2, SC_MASK_FOLDERS); // 0xFE000000 or -33554432
        sci->setMarginSensitiveN(2, true);

        sci->setFoldMarginColour(true, 0xE9E9E9);
        sci->setFoldMarginHiColour(true, 0xFFFFFF);

        sci->setTabWidth(4);
        sci->setUseTabs(false);
        sci->setIndent(4);
        sci->setTabIndents(false);
        sci->setBackSpaceUnIndents(false);
        sci->setIndentationGuides(ptree.get<bool>("show.indent_guide", false) ? SC_IV_REAL : SC_IV_NONE);
        sci->setHighlightGuide(1);
        sci->setPrintMagnification(1);
        sci->setPrintColourMode(0);
        sci->setPrintWrapMode(1);

        initFolderStyle(sci);

        sci->setWrapMode(SC_WRAP_NONE);
        sci->setWrapVisualFlags(ptree.get<bool>("show.wrap_symbol", false) ? SC_WRAPVISUALFLAG_END : SC_WRAPVISUALFLAG_NONE);
        sci->setWrapVisualFlagsLocation(SC_WRAPVISUALFLAGLOC_DEFAULT);
        sci->setWrapStartIndent(0);

        sci->setLayoutCache(SC_CACHE_PAGE);
        sci->linesSplit(0);
        sci->setEdgeMode(0);
        sci->setEdgeColumn(200);
        sci->setEdgeColour(0xC0DCC0);

        sci->usePopUp(false);
        sci->setBufferedDraw(false);
        sci->setPhasesDraw(SC_PHASES_TWO);
        sci->setCodePage(SC_CP_UTF8);
        sci->setWordChars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_");
        sci->setZoom(1);
        sci->setWhitespaceChars(nullptr);
        sci->setMouseDwellTime(2500);

        sci->setSavePoint();
        sci->setFontQuality(SC_EFF_QUALITY_ANTIALIASED);

        int smartIndicator = smartHighlightIndicator();
        sci->indicSetFore(smartIndicator, 0x00FF00);
        sci->indicSetStyle(smartIndicator, INDIC_ROUNDBOX);
        sci->indicSetOutlineAlpha(smartIndicator, 150);
        sci->indicSetAlpha(smartIndicator, 100);
        sci->indicSetUnder(smartIndicator, true);

        // sci:SetEncoding( cfg:GetString("config/encoding") )
        Config *config = Config::instance();
        Q_ASSERT(config);
        QString themePath = config->getThemePath();
        applyThemeStyle(sci, themePath + "/global_style.xml");
    }

    void initFolderStyle(ScintillaEdit *sci)
    {
        Q_ASSERT(sci);
        const int MARK_BOOKMARK = bookmarkMarker();
        sci->markerSetAlpha(MARK_BOOKMARK, 70);
        sci->markerDefine(MARK_BOOKMARK, SC_MARK_BOOKMARK);
        sci->markerSetFore(MARK_BOOKMARK, 0xFF2020);
        sci->markerSetBack(MARK_BOOKMARK, 0xFF2020);
        const int MARGIN = bookmarkMargin();
        int       mask   = sci->marginMaskN(MARGIN);
        sci->setMarginMaskN(MARGIN, (1 << MARK_BOOKMARK) | mask);
        sci->setMarginSensitiveN(MARGIN, true);

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
        sci->setProperty("fold", "1");
        sci->setProperty("fold.flags", "16");
        sci->setProperty("fold.symbols", "1");
        sci->setProperty("fold.compact", "0");
        sci->setProperty("fold.at.else", "1");
        sci->setProperty("fold.preprocessor", "1");
        sci->setProperty("fold.view", "1");
        sci->setProperty("fold.comment", "1");
        sci->setProperty("fold.html", "1");
        sci->setProperty("fold.comment.python", "1");
        sci->setProperty("fold.quotes.python", "1");
    }

    void initEditorStyle(ScintillaEdit *sci, const QString &lang)
    {
        Q_ASSERT(sci);
        Config *config = Config::instance();
        Q_ASSERT(config);

        QString themePath = config->getThemePath();
        applyThemeStyle(sci, themePath + "/global_style.xml");

        void *lexerId = CreateLexer(lang.toStdString().c_str());
        if (lexerId)
        {
            sci->setILexer((sptr_t)lexerId);
            applyThemeStyle(sci, QString("%1/%2.xml").arg(themePath, lang));

            QString langPath = config->getLanguageDirPath();
            applyLanguageStyle(sci, QString("%1/%2.xml").arg(langPath, lang));
        }
        else
        {
#if defined(LOGS_ENABLED)
            qDebug() << Q_FUNC_INFO << __LINE__ << "fallback to lexilla cpp lexer";
#endif
        }

        initFolderStyle(sci);
    }

    void applyLanguageStyle(ScintillaEdit *sci, const QString &configPath)
    {
        Q_ASSERT(sci);
        QDomDocument doc;
        QFile        file(configPath);
        if (!file.open(QIODevice::ReadOnly))
        {
            return;
        }

        BOOST_SCOPE_EXIT(&file)
        {
            file.close();
        }
        BOOST_SCOPE_EXIT_END

        if (!doc.setContent(&file))
        {
            return;
        }

        QDomElement docElem      = doc.documentElement();
        QString     commentLine  = docElem.attribute("comment_line");
        QString     commentStart = docElem.attribute("comment_start");
        QString     commentEnd   = docElem.attribute("comment_end");

        QDomElement keywordElem = docElem.firstChildElement("keyword");
        int         keywordSet  = 0;
        while (!keywordElem.isNull())
        {
            QString name    = keywordElem.attribute("name");
            QString keyword = keywordElem.text();
            sci->setKeyWords(keywordSet++, keyword.toStdString().c_str());
            keywordElem = keywordElem.nextSiblingElement("keyword");
        }
    }

    void applyThemeStyle(ScintillaEdit *sci, const QString &themePath)
    {
        Q_ASSERT(sci);
        QDomDocument doc;
        QFile        file(themePath);
        if (!file.open(QIODevice::ReadOnly))
        {
            return;
        }

        BOOST_SCOPE_EXIT(&file)
        {
            file.close();
        }
        BOOST_SCOPE_EXIT_END

        if (!doc.setContent(&file))
        {
            return;
        }

        QDomElement docElem = doc.documentElement();

        bool zeroId = false;
        for (QDomElement styleElem = docElem.firstChildElement("style"); !styleElem.isNull(); styleElem = styleElem.nextSiblingElement("style"))
        {
            int styleId = styleElem.attribute("style_id").toInt();
            if (styleId == 0)
            {
                // 0 style id used as global override
                // we can skip it if we don't need global override values

                // if (zeroId)
                // {
                continue;
                // }
                // zeroId = true;
            }
            QString foreColor = styleElem.attribute("fg_color");
            if (!foreColor.isEmpty())
            {
                int color = foreColor.toLong(nullptr, 16);
                color     = ((color & 0xFF0000) >> 16) | (color & 0xFF00) | ((color & 0xFF) << 16);
                sci->styleSetFore(styleId, color);
            }
            QString backColor = styleElem.attribute("bg_color");
            if (!backColor.isEmpty())
            {
                int color = backColor.toLong(nullptr, 16);
                color     = ((color & 0xFF0000) >> 16) | (color & 0xFF00) | ((color & 0xFF) << 16);
                sci->styleSetBack(styleId, color);
            }
            QString fontName = styleElem.attribute("font_name");
            if (!fontName.isEmpty())
            {
                sci->styleSetFont(styleId, fontName.toStdString().c_str());
            }
            else
            {
#if defined(Q_OS_MAC)
                sci->styleSetFont(styleId, "Menlo");
#elif defined(Q_OS_WIN)
                sci->styleSetFont(styleId, "Consolas");
#else
                sci->styleSetFont(styleId, "Droid Sans Mono");
#endif
            }

            uint fontStyle = styleElem.attribute("font_style").toUInt();

            std::map<unsigned char, std::function<void(sptr_t, bool)>> styleSetterMap = {
                {0x01, [sci](sptr_t styleId, bool value) { sci->styleSetBold(styleId, value); }},
                {0x02, [sci](sptr_t styleId, bool value) { sci->styleSetItalic(styleId, value); }},
                {0x04, [sci](sptr_t styleId, bool value) { sci->styleSetUnderline(styleId, value); }},
                {0x08, [sci](sptr_t styleId, bool value) { sci->styleSetVisible(styleId, value); }},
                {0x10, [sci](sptr_t styleId, bool value) { sci->styleSetCase(styleId, value); }},
                {0x20, [sci](sptr_t styleId, bool value) { sci->styleSetEOLFilled(styleId, value); }},
                {0x40, [sci](sptr_t styleId, bool value) { sci->styleSetHotSpot(styleId, value); }},
                {0x80, [sci](sptr_t styleId, bool value) { sci->styleSetChangeable(styleId, value); }},
            };
            for (auto &[key, setter] : styleSetterMap)
            {
                if (fontStyle & key)
                {
                    setter(styleId, true);
                }
            }
#if defined(Q_OS_MAC)
            const int defaultFontSize = 16;
#else
            const int defaultFontSize = 12;
#endif
            QString fontSize = styleElem.attribute("font_size");
            if (!fontSize.isEmpty())
            {
                sci->styleSetSize(styleId, std::max(defaultFontSize, fontSize.toInt()));
            }
            else
            {
                sci->styleSetSize(styleId, defaultFontSize);
            }
        }
    }

} // namespace ScintillaConfig
