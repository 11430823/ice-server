#include <stddef.h>
#include <dlfcn.h>

#include <lib_log.h>

#include "ice_dll.h"
#include "service.h"
#include "bind_conf.h"
#include "bench_conf.h"

ice_dll_t g_dll;

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

int ice_dll_t::register_plugin()
{
	char* error; 
	int   ret_code = -1;

	this->m_handle = dlopen(g_bench_conf.get_liblogic_path().c_str(), RTLD_NOW);
	if ((error = dlerror()) != NULL) {
		ALERT_LOG("DLOPEN ERROR [error:%s]", error);
		goto out;
	}

 	DLFUNC(m_handle, functions.on_get_pkg_len, "on_get_pkg_len", int(*)(int, const void*, int, int));
 	DLFUNC(m_handle, functions.on_cli_pkg, "on_cli_pkg", int(*)(void*, int, fdsession_t*));
 	DLFUNC(m_handle, functions.on_srv_pkg, "on_srv_pkg", void(*)(int, void*, int));
 	DLFUNC(m_handle, functions.on_cli_conn_closed, "on_cli_conn_closed", void(*)(int));
 	DLFUNC(m_handle, functions.on_fd_closed, "on_fd_closed", void(*)(int));
   	DLFUNC(m_handle, functions.on_init, "on_init", int(*)(int));
  	DLFUNC(m_handle, functions.on_fini, "on_fini", int(*)(int));
  	DLFUNC(m_handle, functions.on_events, "on_events", void(*)());
	ret_code = 0;

out:
	BOOT_LOG(ret_code, "dlopen [file name:%s, state:%s]",
		g_bench_conf.get_liblogic_path().c_str(), (0 != ret_code ? "FAIL" : "OK"));
}

ice_dll_t::ice_dll_t()
{
	m_handle = NULL;
}

ice_dll_t::~ice_dll_t()
{
	if (NULL != m_handle){
		dlclose(m_handle);
		m_handle = NULL;
	}
}
