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

void service_t::worker_process( int bc_elem_idx, int n_inited_bc )
{
	g_is_parent = false;
	g_epi.EPOLL_TIME_OUT = 100;
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
	g_epi.exit();

	//初始化子进程
	g_epi.init(g_bench_conf.get_max_fd_num());
	g_epi.do_add_conn(m_bind_elem->recv_pipe.pipe_handles[E_PIPE_INDEX_RDONLY], fd_type_pipe, NULL);
	g_epi.start(m_bind_elem->ip.c_str(), m_bind_elem->port, m_bind_elem);

	if ( 0 != g_dll.on_init(g_is_parent)) {
		ALERT_LOG("FAIL TO INIT WORKER PROCESS. [id=%u, name=%s]", m_bind_elem->id, m_bind_elem->name.c_str());
		goto fail;
	}

	g_epi.child_loop(g_bench_conf.get_page_size_max());//mark

fail:
	g_epi.exit();
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

