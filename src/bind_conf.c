#include <glib.h>
#include <glib/gstdio.h>
#include <stdlib.h>

#include "bind_conf.h"
#include "log.h"
#include "daemon.h"

bind_config_t g_bind_conf;

namespace {
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
				elem.online_id = atoi(*value_cursor);
			}
			if (strcmp (*name_cursor, "name") == 0){
				elem.online_name = *value_cursor;
			}
			if (strcmp (*name_cursor, "ip") == 0){
				elem.bind_ip = *value_cursor;
			}
			if (strcmp (*name_cursor, "port") == 0){
				elem.bind_port = atoi(*value_cursor);
				DEBUG_LOG("id:%d, name:%s, ip:%s, port:%d\r\n",
					elem.online_id, elem.online_name.c_str(), 
					elem.bind_ip.c_str(), elem.bind_port);
				g_bind_conf.elems.push_back(elem);
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

	g_file_get_contents( "./bind.xml" , & buf, & length, NULL ); 

	GMarkupParser parser;
	parser.start_element = start;
	parser.end_element = NULL;
	parser.text = NULL;
	parser.passthrough = NULL;
	parser.error = NULL;

	GMarkupParseContext * context; 
	context = g_markup_parse_context_new(&parser, (GMarkupParseFlags)0, NULL, NULL);
	if (!g_markup_parse_context_parse(context, buf, length, NULL)){
		ERROR_LOG("Couldn't load xml\r\n");
	}

	g_markup_parse_context_free(context);
	g_free(buf);

	atomic_t t;
	atomic_set(&t, 0);
	g_daemon.child_pids.resize(elems.size(), t);
}

int bind_config_t::get_bind_conf_idx( const bind_config_elem_t* bc_elem ) const
{
	for (int i = 0 ; i < elems.size(); i++){
		if (elems[i].online_id == bc_elem->online_id){
			return i;
		}
	}
	return -1;
}

uint32_t bind_config_t::get_elem_num() const
{
	return elems.size();
}
