#include <string>
#include "lib_xmlparser.h"

namespace ice {

void load_xmlconf(const char* file, int (*parser)(xmlNodePtr cur_node))
{
	int err = -1;
	xmlKeepBlanksDefault(0); 

	// xmlDocPtr doc = xmlParseFile(file);
	xmlDocPtr doc = xmlReadFile(file, 0, XML_PARSE_NOBLANKS);
	if (!doc) {
		throw XmlParseError(std::string("failed to load file '") + file + "'");
	}

	xmlNodePtr cur = xmlDocGetRootElement(doc);
	if (!cur) {
		xmlFreeDoc(doc);
		throw XmlParseError(std::string("xmlDocGetRootElement error when loading file '") + file + "'");
	}

	err = parser(cur);
	xmlFreeDoc(doc);

	if (err) {
		throw XmlParseError(std::string("failed to parse file '") + file + "'");
	}
}

}//end namespace ice

