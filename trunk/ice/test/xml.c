#include <iostream>
#include <stdint.h>
#include "../libinclude/xmlparser.hpp"

using namespace std;
using namespace ice;

int load_games(xmlNodePtr cur)
{
	cur = cur->xmlChildrenNode; 
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Game"))) {
            uint32_t u;
            float    f;
            string   s;
            char arr[10];
            char name[10];

			get_xml_prop_def(u, cur, "ID", 9999999);
			get_xml_prop_def(f, cur, "MinTime", 0);
			get_xml_prop_def(s, cur, "Name", "def1");
			// ** Note: arr is not null-terminated
			get_xml_prop_arr_def(arr, cur, "Name", 'd');
			get_xml_prop_raw_str_def(name, cur, "Name", "def2");
			cout << u << '\t' << f  << '\t' << s << '\t' << arr << '\t' << name << endl;
			u = 0;
			f = 0;
			s = "";
			arr[0]  = 0;
			name[0] = 0;
			// ** an exception will be thrown if the given prop is not existed
			get_xml_prop(u, cur, "ID");
			get_xml_prop(f, cur, "MinTime");
			get_xml_prop(s, cur, "Name");
			get_xml_prop_arr(arr, cur, "Name");
			get_xml_prop_raw_str(name, cur, "Name");
			cout << u << '\t' << f  << '\t' << s << '\t' << arr << '\t' << name << endl;
		}
		cur = cur->next;
	}
	return 0;
}

int main()
{
    load_xmlconf("xml.xml", load_games);
    return 0;
}
