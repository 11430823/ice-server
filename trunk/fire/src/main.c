#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <lib_util.h>
#include <lib_log.h>
#include <lib_file.h>

#include "daemon.h"
#include "ice_dll.h"
#include "bind_conf.h"
#include "service.h"
#include "bench_conf.h"
#include "ice_epoll.h"

int main(int argc, char* argv[])
{
	if (0 != g_bench_conf.load()){
		BOOT_LOG(-1, "bench conf load ???");
	}

	if (0 != g_bind_conf.load()){
		BOOT_LOG(-1, "bind conf load ???");
	}

	if (0 != g_dll.register_plugin()){
		return -1;
	}

	if (0 != g_net_server.create(10000)){
		BOOT_LOG(-1, "net_server_t::init ???");
	}

	ice::lib_log_t::setup_by_time(g_bench_conf.get_log_dir().c_str(),
		(ice::lib_log_t::E_LEVEL)g_bench_conf.get_log_level(),
		NULL, g_bench_conf.get_log_save_next_file_interval_min());

	g_daemon.prase_args(argc, argv);

	if (0 != g_dll.functions.on_init(g_is_parent)) {
		BOOT_LOG(-1, "FAILED TO INIT PARENT PROCESS");
	}

	for (uint32_t i = 0; i != g_bind_conf.get_elem_num(); ++i ) {
		bind_config_elem_t* bc_elem = g_bind_conf.get_elem(i);
		bc_elem->recv_pipe.create();
		bc_elem->send_pipe.create();
		pid_t pid;
		if ( (pid = fork ()) < 0 ) {
			BOOT_LOG(-1, "fork child process err [id:%u]", bc_elem->id);
		} else if (pid > 0) {
			//父进程
			int ret = ice::lib_file_t::close_fd(g_bind_conf.get_elem(i)->recv_pipe.pipe_handles[E_PIPE_INDEX_RDONLY]);
			ret = ice::lib_file_t::close_fd(g_bind_conf.get_elem(i)->send_pipe.pipe_handles[E_PIPE_INDEX_WRONLY]);
			g_net_server.get_server_epoll()->add_connect(bc_elem->send_pipe.pipe_handles[E_PIPE_INDEX_RDONLY], fd_type_pipe, NULL);
			atomic_set(&g_daemon.child_pids[i], pid);
		} else {
			//子进程
			g_service.run(i, i + 1);
			return 0;
		}
	}

	//parent process
	g_net_server.listen(g_bench_conf.get_daemon_tcp_ip.c_str(), g_bench_conf.get_daemon_tcp_port(), NULL);
	g_net_server.daemon_run();

	g_daemon.killall_children();
	g_net_server.destroy();
	ice::lib_log_t::destroy();
	return 0;
}
