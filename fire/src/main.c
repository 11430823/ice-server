#include <lib_log.h>
#include <lib_file.h>

#include "daemon.h"
#include "dll.h"
#include "bind_conf.h"
#include "service.h"
#include "bench_conf.h"
#include "net_tcp.h"
#include "util.h"

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

	if (0 != g_net_server.create(g_bench_conf.get_daemon_tcp_max_fd_num())){
		BOOT_LOG(-1, "net_server_t::init ???");
	}

	g_daemon.prase_args(argc, argv);

	g_net_server.get_server_epoll()->register_on_functions(&g_dll.functions);
	g_net_server.get_server_epoll()->register_pipe_event_fn(dll_t::on_pipe_event);
	g_net_server.get_server_epoll()->set_epoll_wait_time_out(EPOLL_TIME_OUT);

	ice::lib_log_t::setup_by_time(g_bench_conf.get_log_dir().c_str(),
		(ice::lib_log_t::E_LEVEL)g_bench_conf.get_log_level(),
		NULL, g_bench_conf.get_log_save_next_file_interval_min());
	if (0 != g_dll.functions.on_init(g_is_parent)) {
		BOOT_LOG(-1, "FAILED TO INIT PARENT PROCESS");
	}

	for (uint32_t i = 0; i != g_bind_conf.elems.size(); ++i ) {
		bind_config_elem_t& bc_elem = g_bind_conf.elems[i];
		if (0 != bc_elem.recv_pipe.create()
			|| 0 != bc_elem.send_pipe.create()){
			ERROR_LOG("pipe create err");
			return -1;
		}
		pid_t pid;
		if ( (pid = fork ()) < 0 ) {
			BOOT_LOG(-1, "fork child process err [id:%u]", bc_elem.id);
		} else if (pid > 0) {
			//父进程
			ice::lib_file_t::close_fd(g_bind_conf.elems[i].recv_pipe.handles[E_PIPE_INDEX_RDONLY]);
			ice::lib_file_t::close_fd(g_bind_conf.elems[i].send_pipe.handles[E_PIPE_INDEX_WRONLY]);
			g_net_server.get_server_epoll()->add_connect(bc_elem.send_pipe.handles[E_PIPE_INDEX_RDONLY], ice::FD_TYPE_PIPE, NULL);
			atomic_set(&g_daemon.child_pids[i], pid);
		} else {
			//子进程
			g_service.run(&bc_elem, i + 1);
			return 0;
		}
	}

	//parent process
	if (!g_bench_conf.get_daemon_tcp_ip().empty()){
		if (0 != g_net_server.get_server_epoll()->listen(g_bench_conf.get_daemon_tcp_ip().c_str(), g_bench_conf.get_daemon_tcp_port(), LISTEN_NUM, SEND_RECV_BUF)){
			BOOT_LOG(-1, "daemon listen err [ip:%s, port:%u]", 
				g_bench_conf.get_daemon_tcp_ip().c_str(), g_bench_conf.get_daemon_tcp_port());
		}
	}
	g_daemon.run();

	g_daemon.killall_children();
	g_net_server.destroy();
	ice::lib_log_t::destroy();
	return 0;
}
