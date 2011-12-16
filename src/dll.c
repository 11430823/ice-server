#include <stddef.h>
#include <dlfcn.h>

#include <ice_lib/log.h>

#include "dll.h"
#include "service.h"
#include "bind_conf.h"

dll_t g_dll;

namespace {

	#define DLFUNC(h, v, name, type) \
	{\
		v = (type)dlsym (h, name);\
		if (NULL != (error = dlerror())) {\
			ALERT_LOG("DLSYM ERROR [error:%s, fn:%p]", error, v);\
			dlclose(h);\
			h = NULL;\
			goto out;\
		}\
	}

}//end of namespace

int dll_t::register_plugin( const char* file_name)
{
	char* error; 
	int   ret_code = -1;

	m_handle = dlopen(file_name, RTLD_NOW);
	if ((error = dlerror()) != NULL) {
		ALERT_LOG("DLOPEN ERROR [error:%s]", error);
		goto out;
	}

 	DLFUNC(m_handle, get_pkg_len, "get_pkg_len", int(*)(int, const void*, int, int));
 	DLFUNC(m_handle, proc_pkg_from_client, "proc_pkg_from_client", int(*)(void*, int, fdsession_t*));
 	DLFUNC(m_handle, proc_pkg_from_serv, "proc_pkg_from_serv", void(*)(int, void*, int));
 	DLFUNC(m_handle, on_client_conn_closed, "on_client_conn_closed", void(*)(int));
 	DLFUNC(m_handle, on_fd_closed, "on_fd_closed", void(*)(int));
 
   	DLFUNC(m_handle, init_service, "init_service", int(*)(int));
  	DLFUNC(m_handle, fini_service, "fini_service", int(*)(int));
  	DLFUNC(m_handle, on_events, "on_events", void(*)());
   	DLFUNC(m_handle, proc_mcast_pkg, "proc_mcast_pkg", void(*)(const void*,int));
   	DLFUNC(m_handle, proc_udp_pkg, "proc_udp_pkg", int(*)(int, const void*, int, struct sockaddr_in*, socklen_t));
 	DLFUNC(m_handle, sync_service_info, "sync_service_info", void(*)(uint32_t, const char*, const char*, in_port_t, int));
	ret_code = 0;

out:
	BOOT_LOG(ret_code, "dlopen [file name:%s, state:%s]", file_name, (0 != ret_code ? "FAIL" : "OK"));
	return ret_code;
}

int dll_t::register_data_plugin( const char* file_name )
{
	return 0;//mark
	char* error; 
	int   ret_code = 0;
	if (file_name == NULL){
		return -1;
	}
	m_data_handle = dlopen(file_name, RTLD_NOW | RTLD_GLOBAL);
	if ((error = dlerror()) != NULL) {
		ERROR_LOG("dlopen error, %s", error);
		ret_code = -1;
	}
	ALERT_LOG("dlopen %s %d\r\n", file_name, ret_code);
	return ret_code;
}

void dll_t::unregister_plugin()
{
	if (m_handle != NULL){
		dlclose(m_handle);
		m_handle = NULL;
	}
}

void dll_t::unregister_data_plugin()
{
	if (m_data_handle != NULL){
		dlclose(m_data_handle);
		m_data_handle = NULL;
	}
}

dll_t::dll_t()
{
	m_data_handle = NULL;
	m_handle = NULL;
}