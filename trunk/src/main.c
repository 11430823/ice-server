#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "daemon.h"
#include "dll.h"
#include "net.h"
#include "bind_conf.h"
#include "service.h"
#include "log.h"
#include "bench_conf.h"
#include "shmq.h"

int main(int argc, char* argv[]){
	if (0 != g_bench_conf.load()){
		return -1;
	}

	g_daemon.prase_args(argc, argv);

	if (0 != g_bind_conf.load()){
		return -1;
	}

	//g_dll.register_data_plugin("");
	if (0 != g_dll.register_plugin(g_bench_conf.get_liblogic_path(),e_plugin_flag_load)){
		return -1;
	}

	//asynsvr_init_warning_system();

	if (0 != g_net.init(g_bench_conf.get_max_fd_num(), g_bench_conf.get_max_fd_num())){
		return -1;
	}

	if (g_dll.init_service(1) != 0) {
		SHOW_LOG("FAILED TO INIT PARENT PROCESS");
		ALERT_LOG("FAILED TO INIT PARENT PROCESS");
		return -1;
	}

	pid_t pid;
	for (uint32_t i = 0; i != g_bind_conf.get_elem_num(); ++i ) {
		bind_config_elem_t* bc_elem = g_bind_conf.get_elem(i);
		g_shmq.create(bc_elem);

		if ( (pid = fork ()) < 0 ) {
			SHOW_LOG("fork child process err:-1\r\n");
			return -1;
		} else if (pid > 0) {
			DEBUG_LOG("parent process\r\n");
			g_shmq.close_pipe(&g_bind_conf, i, 0);
			do_add_conn(bc_elem->sendq.pipe_handles[0], fd_type_pipe, 0, bc_elem);			
			net_start(bc_elem->ip.c_str(), bc_elem->port, bc_elem);
			atomic_set(&g_daemon.child_pids[i], pid);
		} else {
			sleep(2);
			DEBUG_LOG("child process\r\n");
			g_listen_port = bc_elem->port;
			g_listen_ip = bc_elem->ip;
			g_service.worker_process(&g_bind_conf, i, i + 1);
		}
	}
//  [9/13/2011 meng]
// 	if (config_get_strval("addr_mcast_ip")) {
// 		if (create_addr_mcast_socket() != 0) {
// 			// return -1 if fail to create mcast socket
// 			BOOT_LOG(-1, "PARENT: FAILED TO CREATE MCAST FOR RELOADING SO");
// 		}
// 	} 

	while (!g_daemon.stop || g_dll.fini_service(1) != 0) {
		net_loop(-1, page_size, 1);
	}
	g_daemon.killall_children();

	net_exit();
	g_dll.unregister_data_plugin();
	g_dll.unregister_plugin();
	shmq_destroy(0, g_bind_conf.get_elem_num());
	return 0;
}