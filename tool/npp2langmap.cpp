#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <string>
#include <list>
#include <direct.h>
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
    std::string name_;
    std::string ext_;
    std::string comment_line_;
    std::string comment_start_;
    std::string comment_end_;
    std::vector<keywords> keywords_list_;
};

void write_language_map(std::list<language>& langs)
{
    rapidxml::xml_document<> doc;
    doc.parse<0>(doc.allocate_string("<AinesmileLanguageMap/>"));
    rapidxml::xml_node<>* root_node = doc.first_node("AinesmileLanguageMap");
    BOOST_ASSERT(root_node);
    for (std::list<language>::iterator it = langs.begin();
        langs.end() != it;
        ++it)
    {
        language& lang = *it;
        rapidxml::xml_node<>* node = doc.allocate_node(rapidxml::node_element, "language");
        node->append_attribute(doc.allocate_attribute("name", doc.allocate_string(lang.name_.c_str())));
        node->append_attribute(doc.allocate_attribute("pattern", ""));
        node->append_attribute(doc.allocate_attribute("suffix", doc.allocate_string(lang.ext_.c_str())));
        root_node->append_node(node);
    }
    std::string filename("langmap.xml");
    std::ofstream ofs(filename.c_str(), std::ios::trunc);
    std::string xmldoc;
    rapidxml::print(std::back_inserter(xmldoc), doc, 0);
    ofs << xmldoc;
    ofs.close();
}

void parse_language_definition(rapidxml::xml_node<>* languages_node)
{
    std::list<language> langs;
    rapidxml::xml_node<>* language_node = languages_node->first_node("Language");
    while(language_node)
    {
        language lang;
        rapidxml::xml_attribute<>* name_attr = language_node->first_attribute("name");
        if (name_attr)
            lang.name_ = name_attr->value();
        rapidxml::xml_attribute<>* ext_attr = language_node->first_attribute("ext");
        if (ext_attr)
            lang.ext_ = ext_attr->value();
        rapidxml::xml_attribute<>* comment_line_attr = language_node->first_attribute("commentLine");
        if (comment_line_attr)
            lang.comment_line_ = comment_line_attr->value();
        rapidxml::xml_attribute<>* comment_start_attr = language_node->first_attribute("commentStart");
        if (comment_start_attr)
            lang.comment_start_ = comment_start_attr->value();
        rapidxml::xml_attribute<>* comment_end_attr = language_node->first_attribute("commentEnd");
        if (comment_end_attr)
            lang.comment_end_ = comment_end_attr->value();

        rapidxml::xml_node<>* keywords_node = language_node->first_node("Keywords");
        while (keywords_node)
        {
            keywords k;
            rapidxml::xml_attribute<>* name_attr = keywords_node->first_attribute("name");
            BOOST_ASSERT(name_attr);
            k.name_ = name_attr->value();
            k.keyword_list_ = keywords_node->value();

            lang.keywords_list_.push_back(k);
            keywords_node = keywords_node->next_sibling("Keywords");
        }

        langs.push_back(lang);
        language_node = language_node->next_sibling("Language");
    }
    write_language_map(langs);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage:\n"
            << "npp2langmap languages.xml" << std::endl;
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
    std::copy(std::istreambuf_iterator<char>(ifs.rdbuf()),
            std::istreambuf_iterator<char>(),
            std::back_inserter(v));
    if (!ifs.good())
    {
        std::cerr << "reading config " << argv[1] << " error." << std::endl;
        return 1;
    }
    v.push_back(0); // zero-terminate
    ifs.close();

    const int flags = rapidxml::parse_normalize_whitespace
        | rapidxml::parse_trim_whitespace
        | rapidxml::parse_comment_nodes;
    rapidxml::xml_document<> doc;
    doc.parse<flags>(&v.front());

    rapidxml::xml_node<>* root_node = doc.first_node("Ainesmile");
    if (!root_node)
    {
        std::cerr << "invalid notepad++ configure file: " << argv[1] << std::endl;
        return 1;
    }

    rapidxml::xml_node<>* type_node = root_node->first_node("Languages");
    if (type_node)
    {
        std::cout << "this is a language definition file" << std::endl;
        parse_language_definition(type_node);
        return 0;
    }

    std::cerr << "this is an unsupported file" << std::endl;

    return 0;
}

