#include <glib.h>
#include <glib/gstdio.h>
#include <string.h>

#include <lib_log.h>
#include <lib_util.h>

#include "daemon.h"
#include "bind_conf.h"

bind_config_t g_bind_conf;

namespace {

	const char* s_bind_config_path =  "./bind.xml";

	void start(GMarkupParseContext *context,
		const gchar *element_name,
		const gchar **attribute_names,
		const gchar **attribute_values,
		gpointer user_data,
		GError **error)
	{
		const gchar **name_cursor = attribute_names;
		const gchar **value_cursor = attribute_values;

		bind_config_elem_t elem;
		while (*name_cursor) {
			if (::strcmp (*name_cursor, "id") == 0){
				elem.id = ::atoi(*value_cursor);
			}
			if (::strcmp (*name_cursor, "name") == 0){
				elem.name = *value_cursor;
			}
			if (::strcmp (*name_cursor, "net_type") == 0){
				elem.net_type = ::atoi(*value_cursor);
			}
			if (::strcmp (*name_cursor, "ip") == 0){
				elem.ip = *value_cursor;
			}
			if (::strcmp (*name_cursor, "port") == 0){
				elem.port = ::atoi(*value_cursor);
				DEBUG_LOG("bind config [id:%d, name:%s, net_type:%d, ip:%s, port:%d]",
					elem.id, elem.name.c_str(), elem.net_type,
					elem.ip.c_str(), elem.port);
				g_bind_conf.elems.push_back(elem);
			}
			name_cursor++;
			value_cursor++;
		}
	}

}//end of namespace

bind_config_elem_t::bind_config_elem_t()
{
	this->id = 0;
	this->port = 0;
	this->restart_cnt = 0;
	this->net_type = 0;
}


int bind_config_t::load()
{
	gchar * buf = NULL; 
	gsize length = 0; 

	g_file_get_contents( s_bind_config_path , &buf, &length, NULL ); 

	GMarkupParser parser;
	parser.start_element = start;
	parser.end_element = NULL;
	parser.text = NULL;
	parser.passthrough = NULL;
	parser.error = NULL;

	GMarkupParseContext * context; 
	context = g_markup_parse_context_new(&parser, (GMarkupParseFlags)0, NULL, NULL);
	if (!g_markup_parse_context_parse(context, buf, length, NULL)){
		BOOT_LOG(-1, "COULDN'T LOAD BIND CONFIG");
	}

	g_markup_parse_context_free(context);
	g_free(buf);

	atomic_t t;
	atomic_set(&t, 0);
	g_daemon.child_pids.resize(this->elems.size(), t);
	return 0;
}

int bind_config_t::get_elem_idx( const bind_config_elem_t* bc_elem )
{
	uint32_t i = 0;
	FOREACH(this->elems, it){
		bind_config_elem_t& elem = *it;
		if (bc_elem->id == elem.id){
			return i;
		}
		i++;		
	}
	return -1;
}
