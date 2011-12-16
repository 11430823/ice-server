#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <ice_lib/util.h>
#include <ice_lib/log.h>

#include "daemon.h"
#include "dll.h"
#include "net.h"
#include "bind_conf.h"
#include "service.h"
#include "bench_conf.h"
#include "shmq.h"

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

	log_init_ex(g_bench_conf.get_log_dir().c_str(), (E_LOG_LEVEL)g_bench_conf.get_log_level(),
		g_bench_conf.get_log_max_byte(), g_bench_conf.get_log_max_files(), NULL,
		g_bench_conf.get_log_save_next_file_interval_min());

// kevinmeng  [2011/10/15 16:58]
#if 0
	g_dll.register_data_plugin("");
#endif
	if (0 != g_dll.register_plugin(g_bench_conf.get_liblogic_path().c_str())){
		return -1;
	}
// kevinmeng  [2011/10/15 16:59]
#if 0
	asynsvr_init_warning_system();
#endif
	if (0 != g_net.init(g_bench_conf.get_max_fd_num(), g_bench_conf.get_max_fd_num())){
		return -1;
	}

	if (g_dll.init_service(1) != 0) {
		ALERT_LOG("FAILED TO INIT PARENT PROCESS");
		return -1;
	}

	for (uint32_t i = 0; i != g_bind_conf.get_elem_num(); ++i ) {
		bind_config_elem_t* bc_elem = g_bind_conf.get_elem(i);
		g_shmq.create(bc_elem);
		pid_t pid;
		if ( (pid = fork ()) < 0 ) {
			ALERT_LOG("fork child process err [id:%u]", bc_elem->id);
			return -1;
		} else if (pid > 0) {
			g_shmq.close_pipe(i, false);
			do_add_conn(bc_elem->sendq.pipe_handles[0], fd_type_pipe, NULL, bc_elem);			
			net_start(bc_elem->ip.c_str(), bc_elem->port, bc_elem);
			atomic_set(&g_daemon.child_pids[i], pid);
		} else {
			g_service.worker_process(i, i + 1);
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

	while (!g_daemon.m_stop || g_dll.fini_service(1) != 0) {
		net_loop(-1, PAGE_SIZE, 1);
	}
	g_daemon.killall_children();
	//TODO 下面没有检查
	net_exit();
	g_dll.unregister_data_plugin();
	g_dll.unregister_plugin();
	shmq_destroy(0, g_bind_conf.get_elem_num());
	log_fini();
	return 0;
}