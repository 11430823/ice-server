#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <lib_log.h>
#include <lib_file.h>

#include "net_tcp.h"
#include "util.h"
#include "daemon.h"
#include "dll.h"
#include "bench_conf.h"
#include "bind_conf.h"
#include "service.h"

service_t g_service;

void service_t::run( bind_config_elem_t* bind_elem, int n_inited_bc )
{
	g_is_parent = false;
	//释放资源(从父进程继承来的资源)
	// close fds inherited from parent process
	g_pipe_fd_elems.clear();
	g_net_server.destroy();
	for (int i = 0; i != n_inited_bc; ++i ) {
		ice::lib_file_t::close_fd(g_bind_conf.elems[i].recv_pipe.handles[E_PIPE_INDEX_WRONLY]);
		ice::lib_file_t::close_fd(g_bind_conf.elems[i].send_pipe.handles[E_PIPE_INDEX_RDONLY]);
	}
	ice::lib_log_t::destroy();

	this->bind_elem = bind_elem;
	char prefix[10] = { 0 };
	int  len = snprintf(prefix, 8, "%u", this->bind_elem->id);
	prefix[len] = '_';	

	ice::lib_log_t::boot(0,0,"ispar:%d, begin setup_by_time::log", g_is_parent);
	ice::lib_log_t::setup_by_time(g_bench_conf.get_log_dir().c_str(),
		(ice::lib_log_t::E_LEVEL)g_bench_conf.get_log_level(),
		prefix, g_bench_conf.get_log_save_next_file_interval_min());

	//初始化子进程
	//todo 处理返回值
	int ret = 0;
	ret = g_net_server.create(g_bench_conf.get_max_fd_num());
	g_net_server.get_server_epoll()->register_on_functions(&g_dll.functions);
	g_net_server.get_server_epoll()->set_epoll_wait_time_out(EPOLL_TIME_OUT);
	ret = g_net_server.get_server_epoll()->add_connect(this->bind_elem->recv_pipe.handles[E_PIPE_INDEX_RDONLY], ice::FD_TYPE_PIPE, NULL);
	ret = g_net_server.get_server_epoll()->listen(LISTEN_NUM);

	if ( 0 != g_dll.functions.on_init(g_is_parent)) {
		ALERT_LOG("FAIL TO INIT WORKER PROCESS. [id=%u, name=%s]", this->bind_elem->id, this->bind_elem->name.c_str());
		goto fail;
	}

	while (!g_daemon.stop || g_dll.functions.on_fini(g_is_parent)){
		g_net_server.get_server_epoll()->run();
		DEBUG_LOG("service_t::run");
	}

fail:
	g_net_server.destroy();
	ice::lib_log_t::destroy();
	exit(0);
}
