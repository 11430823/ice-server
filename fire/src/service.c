#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <lib_util.h>
#include <lib_log.h>
#include <lib_file.h>

#include "ice_epoll.h"
#include "service.h"
#include "bind_conf.h"
#include "daemon.h"
#include "ice_dll.h"
#include "bench_conf.h"

service_t g_service;

void service_t::run( int bc_elem_idx, int n_inited_bc )
{
	g_is_parent = false;

	g_net_server.destroy();

	g_net_server.get_server_epoll()->set_epoll_wait_time_out(100);

	m_bind_elem = g_bind_conf.get_elem(bc_elem_idx);
	char prefix[10] = { 0 };
	int  len = snprintf(prefix, 8, "%u", m_bind_elem->id);
	prefix[len] = '_';
	ice::lib_log_t::setup_by_time(g_bench_conf.get_log_dir().c_str(),
		(ice::lib_log_t::E_LEVEL)g_bench_conf.get_log_level(),
		prefix, g_bench_conf.get_log_save_next_file_interval_min());

	//释放资源(从父进程继承来的资源)
	// close fds inherited from parent process
	for (int i = 0; i != n_inited_bc; ++i ) {
		int ret = ice::lib_file_t::close_fd(g_bind_conf.get_elem(i)->recv_pipe.pipe_handles[E_PIPE_INDEX_WRONLY]);
		ret = ice::lib_file_t::close_fd(g_bind_conf.get_elem(i)->send_pipe.pipe_handles[E_PIPE_INDEX_RDONLY]);
	}

	//初始化子进程
	g_net_server.create(g_bench_conf.get_max_fd_num());
	g_net_server.get_server_epoll->add_connect(m_bind_elem->recv_pipe.pipe_handles[E_PIPE_INDEX_RDONLY], fd_type_pipe, NULL);
	g_net_server.listen(m_bind_elem->ip.c_str(), m_bind_elem->port, m_bind_elem);

	if ( 0 != g_dll.on_init(g_is_parent)) {
		ALERT_LOG("FAIL TO INIT WORKER PROCESS. [id=%u, name=%s]", m_bind_elem->id, m_bind_elem->name.c_str());
		goto fail;
	}

	while (1){
		g_net_server.get_server_epoll()->run();
	}

fail:
	g_net_server.destroy();
	ice::lib_log_t::destroy();
	exit(0);
}

fdsession_t* service_t::get_fdsess( int fd )
{
	std::map<int, fdsession_t*>::iterator it = this->fd_session_map.find(fd);
	if (this->fd_session_map.end() != it){
		return it->second;
	}
	return NULL;
}

service_t::service_t()
{
	this->m_bind_elem = NULL;
}

