#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#if defined(_WIN32)
#    include <direct.h>
#else
#    include <sys/stat.h>
#    include <sys/types.h>
#endif
#include <boost/assert.hpp>

#include "rapidxml.hpp"
#include "rapidxml_print.hpp"

std::string _theme_name;

struct keywords
{
    std::string name_;
    std::string keyword_list_;
};

struct language
{
    std::string           name_;
    std::string           ext_;
    std::string           comment_line_;
    std::string           comment_start_;
    std::string           comment_end_;
    std::vector<keywords> keywords_list_;
};

void write_language_definition(language &lang)
{
    rapidxml::xml_document<> doc;
    doc.parse<0>(doc.allocate_string("<AinesmileLanguage/>"));
    rapidxml::xml_node<> *root_node = doc.first_node("AinesmileLanguage");
    BOOST_ASSERT(root_node);
    root_node->append_attribute(doc.allocate_attribute("name", doc.allocate_string(lang.name_.c_str())));
    root_node->append_attribute(doc.allocate_attribute("pattern", ""));
    root_node->append_attribute(doc.allocate_attribute("suffix", doc.allocate_string(lang.ext_.c_str())));
    root_node->append_attribute(doc.allocate_attribute("comment_line", doc.allocate_string(lang.comment_line_.c_str())));
    root_node->append_attribute(doc.allocate_attribute("comment_start", doc.allocate_string(lang.comment_start_.c_str())));
    root_node->append_attribute(doc.allocate_attribute("comment_end", doc.allocate_string(lang.comment_end_.c_str())));
    for (auto &k : lang.keywords_list_)
    {
        rapidxml::xml_node<> *keyword_node = doc.allocate_node(rapidxml::node_element, "keyword");
        root_node->append_node(keyword_node);
        keyword_node->append_attribute(doc.allocate_attribute("name", doc.allocate_string(k.name_.c_str())));
        keyword_node->value(doc.allocate_string(k.keyword_list_.c_str()));
    }

    std::string   filename("language\\" + lang.name_ + ".xml");
    std::ofstream ofs(filename.c_str(), std::ios::trunc);
    std::string   xmldoc;
    rapidxml::print(std::back_inserter(xmldoc), doc, 0);
    ofs << xmldoc;
    ofs.close();
}

void parse_language_definition(rapidxml::xml_node<> *languages_node)
{
    rapidxml::xml_node<> *language_node = languages_node->first_node("Language");
    while (language_node)
    {
        language                   lang;
        rapidxml::xml_attribute<> *name_attr = language_node->first_attribute("name");
        if (name_attr)
            lang.name_ = name_attr->value();
        rapidxml::xml_attribute<> *ext_attr = language_node->first_attribute("ext");
        if (ext_attr)
            lang.ext_ = ext_attr->value();
        rapidxml::xml_attribute<> *comment_line_attr = language_node->first_attribute("commentLine");
        if (comment_line_attr)
            lang.comment_line_ = comment_line_attr->value();
        rapidxml::xml_attribute<> *comment_start_attr = language_node->first_attribute("commentStart");
        if (comment_start_attr)
            lang.comment_start_ = comment_start_attr->value();
        rapidxml::xml_attribute<> *comment_end_attr = language_node->first_attribute("commentEnd");
        if (comment_end_attr)
            lang.comment_end_ = comment_end_attr->value();

        rapidxml::xml_node<> *keywords_node = language_node->first_node("Keywords");
        while (keywords_node)
        {
            keywords                   k;
            rapidxml::xml_attribute<> *name_attr = keywords_node->first_attribute("name");
            BOOST_ASSERT(name_attr);
            k.name_         = name_attr->value();
            k.keyword_list_ = keywords_node->value();

            lang.keywords_list_.push_back(k);
            keywords_node = keywords_node->next_sibling("Keywords");
        }

        write_language_definition(lang);
        language_node = language_node->next_sibling("Language");
    }
}

struct words_style
{
    std::string name_;
    std::string style_id_;
    std::string fg_color_;
    std::string bg_color_;
    std::string font_name_;
    std::string font_style_;
    std::string font_size_;
};

struct lexer
{
    std::string              name_;
    std::string              description_;
    std::string              ext_;
    std::vector<words_style> words_style_list_;
};

void write_theme_lexer_definition(lexer &l)
{
    std::string file_path(_theme_name + "\\" + l.name_ + ".xml");

    rapidxml::xml_document<> doc;
    doc.parse<0>(doc.allocate_string("<AinesmileTheme/>"));
    rapidxml::xml_node<> *root_node = doc.first_node("AinesmileTheme");
    BOOST_ASSERT(root_node);
    root_node->append_attribute(doc.allocate_attribute("name", doc.allocate_string(l.name_.c_str())));
    root_node->append_attribute(doc.allocate_attribute("desc", doc.allocate_string(l.description_.c_str())));
    root_node->append_attribute(doc.allocate_attribute("ext", doc.allocate_string(l.ext_.c_str())));
    for (auto &ws : l.words_style_list_)
    {
        rapidxml::xml_node<> *style_node = doc.allocate_node(rapidxml::node_element, "style");
        root_node->append_node(style_node);
        style_node->append_attribute(doc.allocate_attribute("name", doc.allocate_string(ws.name_.c_str())));
        style_node->append_attribute(doc.allocate_attribute("style_id", doc.allocate_string(ws.style_id_.c_str())));
        style_node->append_attribute(doc.allocate_attribute("fg_color", doc.allocate_string(ws.fg_color_.c_str())));
        style_node->append_attribute(doc.allocate_attribute("bg_color", doc.allocate_string(ws.bg_color_.c_str())));
        style_node->append_attribute(doc.allocate_attribute("font_name", doc.allocate_string(ws.font_name_.c_str())));
        style_node->append_attribute(doc.allocate_attribute("font_style", doc.allocate_string(ws.font_style_.c_str())));
        style_node->append_attribute(doc.allocate_attribute("font_size", doc.allocate_string(ws.font_size_.c_str())));
    }
    std::ofstream ofs(file_path.c_str(), std::ios::trunc);
    std::string   xmldoc;
    rapidxml::print(std::back_inserter(xmldoc), doc, 0);
    ofs << xmldoc;
    ofs.close();
}

void write_theme_global_definition(std::vector<words_style> &globalstyles)
{
    std::string file_path(_theme_name + "\\global_style.xml");

    rapidxml::xml_document<> doc;
    doc.parse<0>(doc.allocate_string("<AinesmileTheme/>"));
    rapidxml::xml_node<> *root_node = doc.first_node("AinesmileTheme");
    BOOST_ASSERT(root_node);
    for (auto &ws : globalstyles)
    {
        rapidxml::xml_node<> *style_node = doc.allocate_node(rapidxml::node_element, "style");
        root_node->append_node(style_node);
        style_node->append_attribute(doc.allocate_attribute("name", doc.allocate_string(ws.name_.c_str())));
        style_node->append_attribute(doc.allocate_attribute("style_id", doc.allocate_string(ws.style_id_.c_str())));
        style_node->append_attribute(doc.allocate_attribute("fg_color", doc.allocate_string(ws.fg_color_.c_str())));
        style_node->append_attribute(doc.allocate_attribute("bg_color", doc.allocate_string(ws.bg_color_.c_str())));
        style_node->append_attribute(doc.allocate_attribute("font_name", doc.allocate_string(ws.font_name_.c_str())));
        style_node->append_attribute(doc.allocate_attribute("font_style", doc.allocate_string(ws.font_style_.c_str())));
        style_node->append_attribute(doc.allocate_attribute("font_size", doc.allocate_string(ws.font_size_.c_str())));
    }
    std::ofstream ofs(file_path.c_str(), std::ios::trunc);
    std::string   xmldoc;
    rapidxml::print(std::back_inserter(xmldoc), doc, 0);
    ofs << xmldoc;
    ofs.close();
}

void parse_theme_definition(rapidxml::xml_node<> *lexers_node, rapidxml::xml_node<> *globalstyles_node)
{
    rapidxml::xml_node<> *lexer_node = lexers_node->first_node("LexerType");
    while (lexer_node)
    {
        lexer                      l;
        rapidxml::xml_attribute<> *name_attr = lexer_node->first_attribute("name");
        if (name_attr)
            l.name_ = name_attr->value();
        rapidxml::xml_attribute<> *desc_attr = lexer_node->first_attribute("desc");
        if (desc_attr)
            l.description_ = desc_attr->value();
        rapidxml::xml_attribute<> *ext_attr = lexer_node->first_attribute("ext");
        if (ext_attr)
            l.ext_ = ext_attr->value();
        rapidxml::xml_node<> *words_style_node = lexer_node->first_node("WordsStyle");
        while (words_style_node)
        {
            words_style                ws;
            rapidxml::xml_attribute<> *name_attr = words_style_node->first_attribute("name");
            if (name_attr)
                ws.name_ = name_attr->value();
            rapidxml::xml_attribute<> *style_id_attr = words_style_node->first_attribute("styleID");
            if (style_id_attr)
                ws.style_id_ = style_id_attr->value();
            rapidxml::xml_attribute<> *fg_color_attr = words_style_node->first_attribute("fgColor");
            if (fg_color_attr)
                ws.fg_color_ = fg_color_attr->value();
            rapidxml::xml_attribute<> *bg_color_attr = words_style_node->first_attribute("bgColor");
            if (bg_color_attr)
                ws.bg_color_ = bg_color_attr->value();
            rapidxml::xml_attribute<> *font_name_attr = words_style_node->first_attribute("fontName");
            if (font_name_attr)
                ws.font_name_ = font_name_attr->value();
            rapidxml::xml_attribute<> *font_style_attr = words_style_node->first_attribute("fontStyle");
            if (font_style_attr)
                ws.font_style_ = font_style_attr->value();
            rapidxml::xml_attribute<> *font_size_attr = words_style_node->first_attribute("fontSize");
            if (font_size_attr)
                ws.font_size_ = font_size_attr->value();

            l.words_style_list_.push_back(ws);
            words_style_node = words_style_node->next_sibling("WordsStyle");
        }
        write_theme_lexer_definition(l);
        lexer_node = lexer_node->next_sibling("LexerType");
    }

    std::vector<words_style> globalstyles;
    rapidxml::xml_node<>    *words_style_node = globalstyles_node->first_node("WidgetStyle");
    while (words_style_node)
    {
        words_style                ws;
        rapidxml::xml_attribute<> *name_attr = words_style_node->first_attribute("name");
        if (name_attr)
            ws.name_ = name_attr->value();
        rapidxml::xml_attribute<> *style_id_attr = words_style_node->first_attribute("styleID");
        if (style_id_attr)
            ws.style_id_ = style_id_attr->value();
        rapidxml::xml_attribute<> *fg_color_attr = words_style_node->first_attribute("fgColor");
        if (fg_color_attr)
            ws.fg_color_ = fg_color_attr->value();
        rapidxml::xml_attribute<> *bg_color_attr = words_style_node->first_attribute("bgColor");
        if (bg_color_attr)
            ws.bg_color_ = bg_color_attr->value();
        rapidxml::xml_attribute<> *font_name_attr = words_style_node->first_attribute("fontName");
        if (font_name_attr)
            ws.font_name_ = font_name_attr->value();
        rapidxml::xml_attribute<> *font_style_attr = words_style_node->first_attribute("fontStyle");
        if (font_style_attr)
            ws.font_style_ = font_style_attr->value();
        rapidxml::xml_attribute<> *font_size_attr = words_style_node->first_attribute("fontSize");
        if (font_size_attr)
            ws.font_size_ = font_size_attr->value();

        globalstyles.push_back(ws);
        words_style_node = words_style_node->next_sibling("WidgetStyle");
    }

    write_theme_global_definition(globalstyles);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage:\n"
                  << "npp2ainesmile langs.model.xml\n"
                  << "npp2ainesmile theme_file.xml" << std::endl;
        return 1;
    }

    std::ifstream ifs(argv[1]);
    if (!ifs)
    {
        std::cerr << "can't open file " << argv[1] << std::endl;
        return 1;
    }

    ifs.unsetf(std::ios::skipws);
    std::vector<char> v;
    std::copy(std::istreambuf_iterator<char>(ifs.rdbuf()), std::istreambuf_iterator<char>(), std::back_inserter(v));
    if (!ifs.good())
    {
        std::cerr << "reading config " << argv[1] << " error." << std::endl;
        return 1;
    }
    v.push_back(0); // zero-terminate
    ifs.close();

    const int                flags = rapidxml::parse_normalize_whitespace | rapidxml::parse_trim_whitespace | rapidxml::parse_comment_nodes;
    rapidxml::xml_document<> doc;
    doc.parse<flags>(&v.front());

    rapidxml::xml_node<> *root_node = doc.first_node("Ainesmile");
    if (!root_node)
    {
        std::cerr << "invalid notepad++ configure file: " << argv[1] << std::endl;
        return 1;
    }

    rapidxml::xml_node<> *type_node = root_node->first_node("Languages");
    if (type_node)
    {
        std::cout << "this is a language definition file, be sure there is a folder named language, generating language definition files..."
                  << std::endl;
        parse_language_definition(type_node);
        return 0;
    }

    type_node                              = root_node->first_node("LexerStyles");
    rapidxml::xml_node<> *globalstyle_node = root_node->first_node("GlobalStyles");
    if (type_node && globalstyle_node)
    {
        char *dot   = strrchr(argv[1], '.');
        _theme_name = std::string(argv[1], dot - argv[1]);
        _theme_name.append(".asTheme");
#if defined(_WIN32)
        mkdir(_theme_name.c_str());
#else
        mkdir(_theme_name.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
        std::cout << "this is a theme definition file" << std::endl;
        parse_theme_definition(type_node, globalstyle_node);
        return 0;
    }

    std::cerr << "this is an unsupported file" << std::endl;

    return 0;
}
