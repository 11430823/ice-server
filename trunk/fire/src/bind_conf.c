#include <glib.h>
#include <glib/gstdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include <lib_log.h>
#include <lib_util.h>

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
			if (strcmp (*name_cursor, "net_type") == 0){
				elem.net_type = atoi(*value_cursor);
			}
			if (strcmp (*name_cursor, "ip") == 0){
				elem.ip = *value_cursor;
			}
			if (strcmp (*name_cursor, "port") == 0){
				elem.port = atoi(*value_cursor);
				DEBUG_LOG("bind config [id:%d, name:%s, net_type:%d, ip:%s, port:%d]",
					elem.id, elem.name.c_str(), elem.net_type,
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
	g_daemon.child_pids.resize(get_elem_num(), t);
	return 0;
}

int bind_config_t::get_elem_idx( const bind_config_elem_t* bc_elem )
{
	uint32_t i = 0;
	FOREACH(m_elems, it){
		bind_config_elem_t& elem = *it;
		if (bc_elem->id == elem.id){
			return i;
		}
		i++;		
	}
	return -1;
}

bind_config_elem_t* bind_config_t::get_elem( uint32_t index )
{
	return &m_elems[index];
}

void bind_config_t::add_elem(const bind_config_elem_t& elem )
{
	m_elems.push_back(elem);
}

uint32_t bind_config_elem_t::get_id()
{
	return this->id;
}

std::string& bind_config_elem_t::get_name()
{
	return this->name;
}

std::string& bind_config_elem_t::get_ip()
{
	return this->ip;
}

in_port_t bind_config_elem_t::get_port()
{
	return this->port;
}

bind_config_elem_t::bind_config_elem_t()
{
	id = 0;
	port = 0;
	restart_cnt = 0;
	net_type = 0;
}

pipe_t::pipe_t()
{
	::memset(this->pipe_handles, 0, ice::get_arr_num(this->pipe_handles));
}

int pipe_t::create()
{
	if (-1 == ::pipe (this->pipe_handles)){
		ALERT_LOG("PIPE CREATE FAILED [err:%s]", strerror(errno));
		return -1;
	}

	::fcntl (this->pipe_handles[E_PIPE_INDEX_RDONLY], F_SETFL, O_NONBLOCK | O_RDONLY);
	::fcntl (this->pipe_handles[E_PIPE_INDEX_WRONLY], F_SETFL, O_NONBLOCK | O_WRONLY);

	// ��������ΪFD_CLOEXEC��ʾ������ִ��exec����ʱ��fd����ϵͳ�Զ��ر�,��ʾ�����ݸ�exec�������½���
	::fcntl (this->pipe_handles[E_PIPE_INDEX_RDONLY], F_SETFD, FD_CLOEXEC);
	::fcntl (this->pipe_handles[E_PIPE_INDEX_WRONLY], F_SETFD, FD_CLOEXEC);
	return 0;
}