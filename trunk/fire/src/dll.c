#include <dlfcn.h>
#include <sys/stat.h>
#include <stdio.h>

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

	/**
	 * @brief	重命名core文件(core.进程id)
	 */
	void rename_core(int pid)
	{
		::chmod("core", 700);
		::chown("core", 0, 0);
		char core_name[1024] ={0};
		::sprintf(core_name, "core.%d", pid);
		::rename("core", core_name);
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
#include <sys/wait.h>
int dll_t::on_pipe_event( int fd, epoll_event& r_evs )
{
	if (r_evs.events & EPOLLHUP) {
		if (g_is_parent) {
			//////////////////////////////////////////////////////////////////////////
			pid_t pid;
			int	status;
			while ((pid = waitpid (-1, &status, WNOHANG)) > 0) {
				for (uint32_t i = 0; i < g_bind_conf.elems.size(); ++i) {
					if (atomic_read(&g_daemon.child_pids[i]) == pid) {
						atomic_set(&g_daemon.child_pids[i], 0);
						bind_config_elem_t& elem = g_bind_conf.elems[i];
						CRIT_LOG("child process crashed![olid:%u, olname:%s, fd:%d, restart_cnt;%u, restart_cnt_max:%u, pid=%d]",
							elem.id, elem.name.c_str(), fd, elem.restart_cnt, g_bench_conf.get_restart_cnt_max(), pid);
						rename_core(pid);
						// prevent child process from being restarted again and again forever
						if (++elem.restart_cnt <= g_bench_conf.get_restart_cnt_max()) {
							g_daemon.restart_child_process(&elem);
						}else{
							//关闭pipe(不使用PIPE,并且epoll_wait不再监控PIPE)
							ice::lib_file_t::close_fd(elem.recv_pipe.handles[E_PIPE_INDEX_WRONLY]);
							ice::lib_file_t::close_fd(elem.send_pipe.handles[E_PIPE_INDEX_RDONLY]);
						}
						break;
					}
				}
			}
		} else {
			// Parent Crashed
			CRIT_LOG("parent process crashed!");
			g_daemon.stop = true;
			g_daemon.restart = false;
			return 0;
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
