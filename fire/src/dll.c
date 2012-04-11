#include <dlfcn.h>

#include <lib_log.h>

#include "bench_conf.h"
#include "bind_conf.h"
#include "daemon.h"
#include "dll.h"

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

int dll_t::register_plugin()
{
	char* error; 
	int   ret_code = -1;

	this->handle = dlopen(g_bench_conf.get_liblogic_path().c_str(), RTLD_NOW);
	if ((error = dlerror()) != NULL) {
		ALERT_LOG("DLOPEN ERROR [error:%s]", error);
		goto out;
	}

	DLFUNC(this->handle, this->functions.on_get_pkg_len, "on_get_pkg_len",
		ice::on_functions_tcp_server_epoll::ON_GET_PKG_LEN);
	DLFUNC(this->handle, this->functions.on_cli_pkg, "on_cli_pkg",
		ice::on_functions_tcp_server_epoll::ON_CLI_PKG);
 	DLFUNC(this->handle, this->functions.on_srv_pkg, "on_srv_pkg",
		ice::on_functions_tcp_server_epoll::ON_SRV_PKG);
 	DLFUNC(this->handle, this->functions.on_cli_conn_closed, "on_cli_conn_closed",
		ice::on_functions_tcp_server_epoll::ON_CLI_CONN_CLOSED);
 	DLFUNC(this->handle, this->functions.on_svr_conn_closed, "on_svr_conn_closed",
		ice::on_functions_tcp_server_epoll::ON_SVR_CONN_CLOSED);
   	DLFUNC(this->handle, this->functions.on_init, "on_init",
		ice::on_functions_tcp_server_epoll::ON_INIT);
  	DLFUNC(this->handle, this->functions.on_fini, "on_fini",
		ice::on_functions_tcp_server_epoll::ON_FINI);
  	DLFUNC(this->handle, this->functions.on_events, "on_events",
		ice::on_functions_tcp_server_epoll::ON_EVENTS);
	ret_code = 0;

out:
	BOOT_LOG(ret_code, "dlopen [file name:%s, state:%s]",
		g_bench_conf.get_liblogic_path().c_str(), (0 != ret_code ? "FAIL" : "OK"));
}

dll_t::dll_t()
{
	this->handle = NULL;
}

dll_t::~dll_t()
{
	if (NULL != this->handle){
		dlclose(this->handle);
		this->handle = NULL;
	}
}

int dll_t::on_pipe_event( int fd, epoll_event& r_evs )
{
	static int i = 0;
	i++;
	if (r_evs.events & EPOLLHUP) {
		if (g_is_parent) {
			// Child Crashed
			bind_config_elem_t* bc = NULL;
			PIPE_FD_ELEMS_MAP::iterator it = g_pipe_fd_elems.find(fd);
			if (g_pipe_fd_elems.end() != it){
				bc = it->second;
				g_pipe_fd_elems.erase(fd);
				CRIT_LOG("CHILD PROCESS CRASHED![olid:%u, olname:%s, fd:%d, restart_cnt;%u, restart_cnt_max:%u, i:%d]",
					bc->id, bc->name.c_str(), fd, bc->restart_cnt, g_bench_conf.get_restart_cnt_max(), i);

				// prevent child process from being restarted again and again forever
				if (bc->restart_cnt++ <= g_bench_conf.get_restart_cnt_max()) {
					g_daemon.restart_child_process(bc);
				}
			}else{
				CRIT_LOG("CHILD PROCESS CRASHED![olid:??? olname:???]");
			}
		} else {
			// Parent Crashed
			CRIT_LOG("PARENT PROCESS CRASHED!");
			g_daemon.stop = true;
			g_daemon.restart = false;
			return -1;
		}
	} else {
		CRIT_LOG("unuse ???");
		//read
		if (g_is_parent){
		}else{
		}
// 		char trash[trash_size];
// 		while (trash_size == read(fd, trash, trash_size)) ;
	}

	return 0;
}
