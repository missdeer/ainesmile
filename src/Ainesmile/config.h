#ifndef CONFIG_H
#define CONFIG_H

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>

class Config
{
private:
    static Config* instance_;

    boost::property_tree::ptree pt_;
    Config();
public:
    static Config* instance();
    boost::property_tree::ptree& pt();
    void sync();
};

#endif // CONFIG_H
