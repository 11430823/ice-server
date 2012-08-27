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
ice::lib_mcast_t g_mcast;

bool service_check_run()
{
	return likely(!g_daemon.stop) || g_dll.functions.on_fini();
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
		(ice::lib_log_t::E_lOG_LEVEL)g_bench_conf.get_log_level(),
		prefix, g_bench_conf.get_log_save_next_file_interval_min());

	//��ʼ���ӽ���
	int ret = 0;
	if (0 != (ret = g_net_server.create(g_bench_conf.get_max_fd_num()))){
		ALERT_LOG("g_net_server.create err [ret:%d]", ret);
		return;
	}
	tcp_server_epoll_t* ser = g_net_server.get_server_epoll();
	ser->register_on_functions(&g_dll.functions);
	ser->set_on_pipe_event(dll_t::on_pipe_event);
	ser->set_epoll_wait_time_out(EPOLL_TIME_OUT);

	ser->add_connect(this->bind_elem->recv_pipe.handles[E_PIPE_INDEX_RDONLY], ice::FD_TYPE_PIPE, NULL, 0);

	if (0 != ser->listen(this->bind_elem->ip.c_str(),
		this->bind_elem->port, g_bench_conf.get_listen_num(), g_bench_conf.get_page_size_max())){
		BOOT_LOG_VOID(-1, "server listen err [ip:%s, port:%u]", this->bind_elem->ip.c_str(), this->bind_elem->port);
	}

	if ( 0 != g_dll.functions.on_init()) {
		ALERT_LOG("FAIL TO INIT WORKER PROCESS. [id=%u, name=%s]", this->bind_elem->id, this->bind_elem->name.c_str());
		goto fail;
	}

	//�����鲥
	if (!g_bench_conf.get_mcast_ip().empty()){
		if (0 != g_mcast.create(g_bench_conf.get_mcast_ip(), 
			g_bench_conf.get_mcast_port(), g_bench_conf.get_mcast_incoming_if(),
			g_bench_conf.get_mcast_outgoing_if())){
				ALERT_LOG("mcast.create err[ip:%s]", g_bench_conf.get_mcast_ip().c_str());
			return;
		}else{
			ser->add_connect(g_mcast.get_fd(),
				ice::FD_TYPE_MCAST, g_bench_conf.get_mcast_ip().c_str(), g_bench_conf.get_mcast_port());
		}
	}

	//������ַ��Ϣ�鲥
	if (!g_bench_conf.get_addr_mcast_ip().empty()){
		if (0 != g_addr_mcast.create(g_bench_conf.get_addr_mcast_ip(),
			g_bench_conf.get_addr_mcast_port(), g_bench_conf.get_addr_mcast_incoming_if(),
			g_bench_conf.get_addr_mcast_outgoing_if())){
				ALERT_LOG("addr mcast.create err [ip:%s]", g_bench_conf.get_addr_mcast_ip().c_str());
				return;
		} else {
			ser->add_connect(g_addr_mcast.get_fd(),
				ice::FD_TYPE_ADDR_MCAST, g_bench_conf.get_addr_mcast_ip().c_str(), g_bench_conf.get_addr_mcast_port());
			g_addr_mcast.mcast_notify_addr(MCAST_CMD_ADDR_1ST);
		}
	}

	ser->run(service_check_run);

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