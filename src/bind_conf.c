#include <glib.h>
#include <glib/gstdio.h>
#include <stdlib.h>
#include <string.h>

#include <ice_lib/log.h>

#include "bind_conf.h"
#include "daemon.h"

bind_config_t g_bind_conf;

namespace {

	const char* bind_config_path =  "./bind.xml";

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
			if (strcmp (*name_cursor, "id") == 0){
				elem.id = atoi(*value_cursor);
			}
			if (strcmp (*name_cursor, "name") == 0){
				elem.name = *value_cursor;
			}
			if (strcmp (*name_cursor, "ip") == 0){
				elem.ip = *value_cursor;
			}
			if (strcmp (*name_cursor, "port") == 0){
				elem.port = atoi(*value_cursor);
				DEBUG_LOG("bind config [id:%d, name:%s, ip:%s, port:%d]",
					elem.id, elem.name.c_str(), 
					elem.ip.c_str(), elem.port);
				g_bind_conf.add_elem(elem);
			}
			name_cursor++;
			value_cursor++;
		}
	}

}//end of namespace

int bind_config_t::load()
{
	gchar * buf = NULL; 
	gsize length = 0; 

	g_file_get_contents( bind_config_path , &buf, &length, NULL ); 

	GMarkupParser parser;
	parser.start_element = start;
	parser.end_element = NULL;
	parser.text = NULL;
	parser.passthrough = NULL;
	parser.error = NULL;

	GMarkupParseContext * context; 
	context = g_markup_parse_context_new(&parser, (GMarkupParseFlags)0, NULL, NULL);
	if (!g_markup_parse_context_parse(context, buf, length, NULL)){
		ALERT_LOG("COULDN'T LOAD BIND CONFIG");
		return -1;
	}

	g_markup_parse_context_free(context);
	g_free(buf);

	atomic_t t;
	atomic_set(&t, 0);
	g_daemon.child_pids.resize(g_bind_conf.get_elem_num(), t);
	return 0;
}

int bind_config_t::get_bind_conf_idx( const bind_config_elem_t* bc_elem ) const
{
	for (uint32_t i = 0 ; i < elems.size(); i++){
		if (elems[i].id == bc_elem->id){
			return i;
		}
	}
	return -1;
}

bind_config_elem_t* bind_config_t::get_elem( uint32_t index )
{
	return &elems[index];
}

void bind_config_t::add_elem(const bind_config_elem_t& elem )
{
	elems.push_back(elem);
}

bind_config_elem_t::bind_config_elem_t()
{
	id = 0;
	port = 0;
	restart_cnt = 0;
}
