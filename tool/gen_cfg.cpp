#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <string>
#include <vector>
#if defined(WIN32)
#include <direct.h>
#else
#include <dirent.h>
#endif
#include <boost/assert.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/info_parser.hpp>
#include "rapidxml.hpp"
#include "rapidxml_print.hpp"

int main(int argc, char *argv[])
{
    // Create an empty property tree object
    using boost::property_tree::ptree;
    ptree pt;

    pt.put("hscrollbar", true);
    pt.put("vscrollbar", true);
    pt.put("xoffset", 0);
    pt.put("scrollwidth", 1);
    pt.put("scrollwidthtracking", true);
    pt.put("endatlastline", true);
    pt.put("whitespacefore.enable", true);
    pt.put("whitespacefore.color", 0x808080);
    pt.put("whitespaceback.enable", true);
    pt.put("whitespaceback.color", 0xFFFFFF);
    pt.put("mousedowncapture", true);
    pt.put("vieweol", false);
    pt.put("stylebits", 5);
    pt.put("caret.fore", 0x0000FF);
    pt.put("caret.linevisible", true);
    pt.put("caret.lineback", 0xFFFFD0);
    pt.put("caret.linebackalpha", 256);
    pt.put("caret.period", 500);
    pt.put("caret.width", 2);
    pt.put("caret.sticky", 0);
    pt.put("sel.fore.enable", true);
    pt.put("sel.fore.color", 0xFFFFFF);
    pt.put("sel.back.enable", true);
    pt.put("sel.back.color", 0xC56A31);
    pt.put("sel.alpha", 256);
    pt.put("sel.eolfilled", true);
    pt.put("additionalselectiontyping", true);
    pt.put("virtualspaceoption", 0);
    pt.put("hotspot.active.fore.enable", true);
    pt.put("hotspot.active.fore.color", 0x0000FF);
    pt.put("hotspot.active.back.enable", true);
    pt.put("hotspot.active.back.color", 0xFFFFFF);
    pt.put("hotspot.active.underline", true);
    pt.put("hotspot.singleline", false);
    pt.put("controlcharsymbol", 0);

    pt.put("fold.margin.color.enable", true);
    pt.put("fold.margin.color.color", 0xCDCDCD);
    pt.put("fold.margin.hicolor.enable", true);
    pt.put("fold.margin.hicolor.color", 0xFFFFFF);

    pt.put("tabwidth", 4);
    pt.put("usetabs", false);
    pt.put("indent", 4);
    pt.put("tabindents", false);
    pt.put("backspaceunindents", false);
    pt.put("indentationguids", 0);
    pt.put("highlightguid", 1);
    pt.put("print.magnification", 1);
    pt.put("print.colormode", 0);
    pt.put("print.wrapmode", 1);

    pt.put("wrap.mode", 0);
    pt.put("wrap.visualflags", 0);
    pt.put("wrap.visualflagslocation", 0);
    pt.put("wrap.startindent", 0);

    pt.put("layoutcache", 2);
    pt.put("linessplit", 0);
    pt.put("edge.mode", 0);
    pt.put("edge.column", 200);
    pt.put("edge.color", 0xC0DCC0);

    pt.put("codepage", 0);
    pt.put("zoom", 1);
    pt.put("mousedwelltime", 2500);


    pt.put("theme", "Default");

    // Write the property tree to the XML file.
    write_xml(".Ainesmilerc.xml", pt);
    write_json(".Ainesmilerc.json", pt);
    write_info(".Ainesmilerc.info", pt);
    //write_ini(".Ainesmilerc.ini", pt);

    return 0;
}

