#include <lib_log.h>
#include "route.h"

route_t g_rotue_t;

const char* ROUTE_XML_PATH = "./route.xml";

namespace {
	int parser_cmd(xmlNodePtr cur, ice::lib_xmlparser& xml, uint32_t db_type){
		cur = cur->xmlChildrenNode;
		while (NULL != cur){
			if (!xmlStrcmp(cur->name,(const xmlChar*)"date")){
				route_cmd_t info;
				info.db_type = db_type;

				xml.get_xml_prop(cur, info.start, "start");
				INFO_LOG("start:%u", info.start);
				xml.get_xml_prop(cur, info.end, "end");
				INFO_LOG("end:%u", info.end);
				route_t::DB_SER dbser;
				if (!g_rotue_t.cmd_map.insert(std::make_pair(info, dbser)).second){
					assert(0);
					return -1;
				}
			}
			cur = cur->next;
		}
		return 0;
	}

	int parser_db(xmlNodePtr cur, ice::lib_xmlparser& xml){
		cur = cur->xmlChildrenNode;
		while (NULL != cur){
			if (!xmlStrcmp(cur->name,(const xmlChar*)"date")){
				db_info_t dbinfo;
				xml.get_xml_prop(cur, dbinfo.name, "name");
				INFO_LOG("name:%s", dbinfo.name.c_str());
				xml.get_xml_prop(cur, dbinfo.ip, "ip");
				INFO_LOG("ip:%s", dbinfo.ip.c_str());

				route_db_t db;
				xml.get_xml_prop(cur, db.start, "start");
				INFO_LOG("start:%u", db.start);
				xml.get_xml_prop(cur, db.end, "end");
				INFO_LOG("end:%u", db.end);

				FOREACH(g_rotue_t.cmd_map, it){
					route_t::DB_SER& dbser = it->second;
					if (!dbser.insert(std::make_pair(db, dbinfo)).second){
						assert(0);
						return -1;
					}
				}
			}
			cur = cur->next;
		}
		return 0;
	}

	int parser_dbser(xmlNodePtr cur, ice::lib_xmlparser& xml, uint32_t db_type){
		cur = cur->xmlChildrenNode;
		while(NULL != cur){
			if(!xmlStrcmp(cur->name, (const xmlChar*)"cmd")){
				parser_cmd(cur, xml, db_type);
			} else if(!xmlStrcmp(cur->name, (const xmlChar*)"db")){
				parser_db(cur, xml);
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
			xml.get_xml_prop(cur, db_type,"type");
			INFO_LOG("db_type:%u", db_type);
			if (E_DB_TYPE_1 != db_type && E_DB_TYPE_100 != db_type){
				assert(0);
				return -1;
			}

			parser_dbser(cur, this->xml, db_type);
		}
		xml.move2next_node();
	}
	return 0;
}
