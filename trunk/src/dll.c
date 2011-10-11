#include <stddef.h>
#include <dlfcn.h>

#include "dll.h"
#include "service.h"
#include "bind_conf.h"
#include "log.h"

dll_t g_dll;

#define DLFUNC(h, v, name, type) \
	do { \
		v = (type)dlsym (h, name);\
		if ((error = dlerror ()) != NULL) { \
			ERROR_LOG("dlsym error, %s %p\r\n", error, v);\
			dlclose(h); \
			h = NULL; \
			goto out; \
		} \
	} while (0)

uint32_t get_server_id()
{
	return config_cache.bc_elem->online_id;
}

uint32_t get_cli_ip2( int fd )
{
	fdsession_t* fdsess = (fdsession_t*)g_hash_table_lookup(fds.cn, &fd);
	return (fdsess ? fdsess->remote_ip : 0);
}

uint32_t get_cli_port( const fdsession_t* fdsess )
{
	return fdsess->remote_port;
}

uint32_t get_cli_ip( const fdsession_t* fdsess )
{
	return fdsess->remote_ip;
}

in_port_t get_server_port()
{
	return config_cache.bc_elem->bind_port;
}

const char* get_server_ip()
{
	return config_cache.bc_elem->bind_ip.c_str();
}

const char* get_server_name()
{
	return config_cache.bc_elem->online_name.c_str();
}

int dll_t::register_plugin( const char* file_name, E_PLUGIN_FLAG flag )
{
	char* error; 
	int   ret_code = -1;

	g_dll.handle = dlopen(file_name, RTLD_NOW);
	if ((error = dlerror()) != NULL) {
		ERROR_LOG("dlopen error, %s \r\n", error);
		goto out;
	}
 	DLFUNC(g_dll.handle, g_dll.get_pkg_len, "get_pkg_len", int(*)(int, const void*, int, int));
 	DLFUNC(g_dll.handle, g_dll.proc_pkg_from_client, "proc_pkg_from_client", int(*)(void*, int, fdsession_t*));
 	DLFUNC(g_dll.handle, g_dll.proc_pkg_from_serv, "proc_pkg_from_serv", void(*)(int, void*, int));
 	DLFUNC(g_dll.handle, g_dll.on_client_conn_closed, "on_client_conn_closed", void(*)(int));
 	DLFUNC(g_dll.handle, g_dll.on_fd_closed, "on_fd_closed", void(*)(int));
 
   	DLFUNC(g_dll.handle, g_dll.init_service, "init_service", int(*)(int));
  	DLFUNC(g_dll.handle, g_dll.fini_service, "fini_service", int(*)(int));
  	DLFUNC(g_dll.handle, g_dll.on_events, "on_events", void(*)());
   	DLFUNC(g_dll.handle, g_dll.proc_mcast_pkg, "proc_mcast_pkg", void(*)(const void*,int));
   	DLFUNC(g_dll.handle, g_dll.proc_udp_pkg, "proc_udp_pkg", int(*)(int, const void*, int, struct sockaddr_in*, socklen_t));
 	DLFUNC(g_dll.handle, g_dll.reload_global_data, "reload_global_data", int(*)(void));
 	DLFUNC(g_dll.handle, g_dll.sync_service_info, "sync_service_info", void(*)(uint32_t, const char*, const char*, in_port_t, int));
	ret_code = 0;

out:
	if (!flag) {
		SHOW_LOG("dlopen %s, %d\r\n", file_name, ret_code);
	} else {
		DEBUG_LOG("RELOAD %s\t[%s]\r\n", file_name, (ret_code ? "FAIL" : "OK"));
		return ret_code;
	}
	return 0;
}

int dll_t::register_data_plugin( const char* file_name )
{
	char* error; 
	int   ret_code = 0;
	if (file_name == NULL){
		return -1;
	}
	data_handle = dlopen(file_name, RTLD_NOW | RTLD_GLOBAL);
	if ((error = dlerror()) != NULL) {
		ERROR_LOG("dlopen error, %s", error);
		ret_code = -1;
	}
	SHOW_LOG("dlopen %s %d\r\n", file_name, ret_code);
	return ret_code;
}

void dll_t::unregister_plugin()
{
	if (handle != NULL){
		dlclose(handle);
		handle = NULL;
	}
}

void dll_t::unregister_data_plugin()
{
	if (data_handle != NULL){
		dlclose(data_handle);
		data_handle = NULL;
	}
}
