
#include <lib_log.h>
#include "route.h"

route_t g_rotue_t;

const char* ROUTE_XML_PATH = "./route.xml";

namespace {
	int parser_cmd_id(xmlNodePtr cur, ice::lib_xmlparser& xml){
		cur = cur->xmlChildrenNode;
		while (NULL != cur){
			if (!xmlStrcmp(cur->name,(const xmlChar*)"date")){
				uint32_t start = 0;
				xml.get_xml_prop(cur, start, "start");
				INFO_LOG("start:%u", start);
				uint32_t end = 0;
				xml.get_xml_prop(cur, end, "end");
				INFO_LOG("end:%u", end);
			}
			cur = cur->next;
		}
		return 0;
	}
	int parser_connect_db(xmlNodePtr cur, ice::lib_xmlparser& xml){
		cur = cur->xmlChildrenNode;
		while (NULL != cur){
			if (!xmlStrcmp(cur->name,(const xmlChar*)"date")){
				std::string name;
				xml.get_xml_prop(cur, name, "name");
				INFO_LOG("name:%s", name.c_str());
				std::string ip;
				xml.get_xml_prop(cur, ip, "ip");
				INFO_LOG("ip:%s", ip.c_str());
				uint32_t start_id = 0;
				xml.get_xml_prop(cur, start_id, "start_id");
				INFO_LOG("start_id:%u", start_id);
				uint32_t end_id = 0;
				xml.get_xml_prop(cur, end_id, "end_id");
				INFO_LOG("end_id:%u", end_id);
			}
			cur = cur->next;
		}
		return 0;
	}
	int parser_dbser(xmlNodePtr cur, ice::lib_xmlparser& xml){
		cur = cur->xmlChildrenNode;

		while(NULL != cur){
			if(!xmlStrcmp(cur->name, (const xmlChar*)"cmd_id")){
				parser_cmd_id(cur, xml);
			} else if(!xmlStrcmp(cur->name, (const xmlChar*)"connect_db")){
				parser_connect_db(cur, xml);
			}
			cur = cur->next;
		}
		return 0;
	}
}
int route_t::parser()
{
	int ret = this->xml.open(ROUTE_XML_PATH);
	if (0 != ret){
		return ret;
	}

	this->xml.move2children_node();
	while(NULL != xml.get_node_ptr()){
		//取出节点中的内容
		if (!xmlStrcmp(xml.get_node_ptr()->name, (const xmlChar *)"dbser")){
			xmlNodePtr cur = this->xml.get_node_ptr();

			uint32_t db_type = 0;
			xml.get_xml_prop(cur, db_type,"db_type");
			INFO_LOG("db_type:%u", db_type);

			parser_dbser(cur, this->xml);
		}
		xml.move2next_node();
	}
	return 0;
}
