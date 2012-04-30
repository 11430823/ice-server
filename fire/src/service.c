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

bool service_check_run()
{
	return likely(!g_daemon.stop) || g_dll.functions.on_fini(g_is_parent);
}

void service_t::run( bind_config_elem_t* bind_elem, int n_inited_bc )
{
	g_is_parent = false;
	//�ͷ���Դ(�Ӹ����̼̳�������Դ)
	// close fds inherited from parent process
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
	ice::lib_log_t::setup_by_time(g_bench_conf.get_log_dir().c_str(),
		(ice::lib_log_t::E_LEVEL)g_bench_conf.get_log_level(),
		prefix, g_bench_conf.get_log_save_next_file_interval_min());

	//��ʼ���ӽ���
	int ret = 0;
	if (0 != (ret = g_net_server.create(g_bench_conf.get_max_fd_num()))){
		ALERT_LOG("g_net_server.create err [ret:%d]", ret);
		return;
	}
	g_net_server.get_server_epoll()->register_on_functions(&g_dll.functions);
	g_net_server.get_server_epoll()->set_on_pipe_event(dll_t::on_pipe_event);
	g_net_server.get_server_epoll()->set_epoll_wait_time_out(EPOLL_TIME_OUT);

	g_net_server.get_server_epoll()->add_connect(this->bind_elem->recv_pipe.handles[E_PIPE_INDEX_RDONLY], ice::FD_TYPE_PIPE, NULL, 0);

	//�����鲥
	if (!g_bench_conf.get_mcast_ip().empty()){
		if (0 != this->mcast.create(g_bench_conf.get_mcast_ip(), 
			g_bench_conf.get_mcast_port(), g_bench_conf.get_mcast_incoming_if(),
			g_bench_conf.get_mcast_outgoing_if())){
			ALERT_LOG("mcast.create err");
			return;
		}else{
			g_net_server.get_server_epoll()->add_connect(this->mcast.get_fd(),
				ice::FD_TYPE_MCAST, g_bench_conf.get_mcast_ip().c_str(), g_bench_conf.get_mcast_port());
		}
	}

	//������ַ��Ϣ�鲥
	if (!g_bench_conf.get_addr_mcast_ip().empty()){
		if (0 != this->addr_mcast.create(g_bench_conf.get_addr_mcast_ip(),
			g_bench_conf.get_addr_mcast_port(), g_bench_conf.get_addr_mcast_incoming_if(),
			g_bench_conf.get_addr_mcast_outgoing_if())){
			ALERT_LOG("addr mcast.create err");
			return;
		} else {
			g_net_server.get_server_epoll()->add_connect(this->addr_mcast.get_fd(),
				ice::FD_TYPE_ADDR_MCAST, g_bench_conf.get_addr_mcast_ip().c_str(), g_bench_conf.get_addr_mcast_port());
			this->addr_mcast.mcast_notify_addr(g_service.get_bind_elem_id(), 
				g_service.get_bind_elem_name(), g_service.bind_elem->ip.c_str(),
				g_service.bind_elem->port, ice::lib_addr_multicast_t::ADDR_MCAST_1ST_PKG);
			g_net_server.get_server_epoll()->set_addr_mcast(&this->addr_mcast);
		}
	}

	if (0 != g_net_server.get_server_epoll()->listen(this->bind_elem->ip.c_str(), this->bind_elem->port, LISTEN_NUM, SEND_RECV_BUF)){
		BOOT_LOG_VOID(-1, "server listen err [ip:%s, port:%u]", this->bind_elem->ip.c_str(), this->bind_elem->port);
	}

	if ( 0 != g_dll.functions.on_init(g_is_parent)) {
		ALERT_LOG("FAIL TO INIT WORKER PROCESS. [id=%u, name=%s]", this->bind_elem->id, this->bind_elem->name.c_str());
		goto fail;
	}

	g_net_server.get_server_epoll()->run(service_check_run);

fail:
	g_net_server.destroy();
	ice::lib_log_t::destroy();
	exit(0);
}

uint32_t service_t::get_bind_elem_id()
{
	return this->bind_elem->id;
}

const char* service_t::get_bind_elem_name()
{
	return this->bind_elem->name.c_str();
}
