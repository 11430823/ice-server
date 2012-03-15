#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <lib_util.h>
#include <lib_log.h>

#include "daemon.h"
#include "ice_dll.h"
#include "bind_conf.h"
#include "service.h"
#include "bench_conf.h"
#include "shmq.h"
#include "ice_epoll.h"

namespace {
	int load_config(){
		if (0 != g_bench_conf.load()){
			return -1;
		}

		if (0 != g_bind_conf.load()){
			return -1;
		}
		return 0;
	}
}

int main(int argc, char* argv[]){
	if(0 != load_config()){
		return -1;
	}

	g_daemon.prase_args(argc, argv);

	ice::lib_log_t::setup_by_time(g_bench_conf.get_log_dir().c_str(), (ice::lib_log_t::E_LEVEL)g_bench_conf.get_log_level(),
		NULL, g_bench_conf.get_log_save_next_file_interval_min());

	if (0 != g_dll.register_plugin()){
		return -1;
	}
	g_epi.init(g_bench_conf.get_max_fd_num(), g_bench_conf.get_max_fd_num());

	if (0 != g_dll.on_init(1)) {
		ALERT_LOG("FAILED TO INIT PARENT PROCESS");
		return -1;
	}

	for (uint32_t i = 0; i != g_bind_conf.get_elem_num(); ++i ) {
		bind_config_elem_t* bc_elem = g_bind_conf.get_elem(i);
		bc_elem->recv_pipe.create();
		bc_elem->send_pipe.create();
		pid_t pid;
		if ( (pid = fork ()) < 0 ) {
			ALERT_LOG("fork child process err [id:%u]", bc_elem->id);
			return -1;
		} else if (pid > 0) {
			//父进程
			pipe_t::close_pipe(i, g_is_parent);
			g_epi.do_add_conn(bc_elem->send_pipe.pipe_handles[E_PIPE_INDEX_RDONLY], fd_type_pipe, NULL, bc_elem);			
			atomic_set(&g_daemon.child_pids[i], pid);
		} else {
			//子进程
			g_service.worker_process(i, i + 1);
			return 0;
		}
	}
	//parent process

	//  [9/13/2011 meng]
#if 0
	if (config_get_strval("addr_mcast_ip")) {
		if (create_addr_mcast_socket() != 0) {
			// return -1 if fail to create mcast socket
			BOOT_LOG(-1, "PARENT: FAILED TO CREATE MCAST FOR RELOADING SO");
		}
	} 
#endif

	while (!g_daemon.m_stop || g_dll.on_fini(g_is_parent) != 0) {
		g_epi.loop(g_bench_conf.get_page_size_max());
	}
	g_daemon.killall_children();
	net_exit();
	ice::lib_log_t::destroy();
	return 0;
}
