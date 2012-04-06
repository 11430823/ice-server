#include <stddef.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>

#include <lib_util.h>
#include <lib_log.h>
#include <lib_file.h>
#include <lib_tcp_server.h>

#include "bind_conf.h"
#include "bench_conf.h"
#include "service.h"
#include "dll.h"
#include "net_tcp.h"
#include "daemon.h"

daemon_t g_daemon;
bool g_is_parent = true;
PIPE_FD_ELEMS_MAP g_pipe_fd_elems;

namespace {

	std::vector<std::string> g_argvs;

	void sigterm_handler(int signo) 
	{
		//停止服务（不重启）
		ALERT_LOG("SIG_TERM FROM [pid=%d, is_parent:%d]", getpid(), (int)g_is_parent);
		g_daemon.stop     = true;
		g_daemon.restart  = false;
	}

	void sighup_handler(int signo) 
	{
		//停止&&重启服务
		ALERT_LOG("SIGHUP FROM [pid=%d]", getpid());
		g_daemon.restart  = true;
		g_daemon.stop     = true;
	}

	void sigchld_handler(int signo, siginfo_t *si, void * p) 
	{
		ALERT_LOG("SIGCHLD FROM [pid=%d, is_parent:%d]", getpid(), (int)g_is_parent);
		pid_t pid;
		static int	status;
		while ((pid = waitpid (-1, &status, WNOHANG)) > 0) {
			for (uint32_t i = 0; i < g_bind_conf.elems.size(); ++i) {
				if (atomic_read(&g_daemon.child_pids[i]) == pid) {
					atomic_set(&g_daemon.child_pids[i], 0);
					//todo g_daemon.restart_child_process(bc);将重启的功能放在这里
					break;
				}
			}
		}
	}

	void rlimit_reset()
	{
		struct rlimit rlim;

		/* set open files */
		rlim.rlim_cur = g_bench_conf.get_max_fd_num();
		rlim.rlim_max = g_bench_conf.get_max_fd_num();
		if (-1 == setrlimit(RLIMIT_NOFILE, &rlim)) {
			ALERT_LOG("INIT FD RESOURCE FAILED [OPEN FILES NUMBER:%d]", g_bench_conf.get_max_fd_num());
		}

		/* set core dump */
		rlim.rlim_cur = g_bench_conf.get_core_size();
		rlim.rlim_max = g_bench_conf.get_core_size();
		if (-1 == setrlimit(RLIMIT_CORE, &rlim)) {
			ALERT_LOG("INIT CORE FILE RESOURCE FAILED [CORE DUMP SIZE:%d]", g_bench_conf.get_core_size());
		}
	}

	void save_argv(int argc, char** argv)
	{
		for (int i = 0; i < argc; i++){
			std::string str = argv[i];
			g_argvs.push_back(str);
		}
	}

	void set_signal()
	{
		struct sigaction sa;
		memset(&sa, 0, sizeof(sa));

		sigset_t sset;
		signal(SIGPIPE,SIG_IGN);

		sa.sa_handler = sigterm_handler;
		sigaction(SIGINT, &sa, NULL);
		sigaction(SIGTERM, &sa, NULL);
		sigaction(SIGQUIT, &sa, NULL);

		sa.sa_handler = sighup_handler;
		sigaction(SIGHUP, &sa, NULL);

		sa.sa_flags = SA_RESTART|SA_SIGINFO;
		sa.sa_sigaction = sigchld_handler;
		sigaction(SIGCHLD, &sa, NULL);

		sigemptyset(&sset);
		sigaddset(&sset, SIGSEGV);
		sigaddset(&sset, SIGBUS);
		sigaddset(&sset, SIGABRT);
		sigaddset(&sset, SIGILL);
		sigaddset(&sset, SIGCHLD);
		sigaddset(&sset, SIGFPE);
		sigprocmask(SIG_UNBLOCK, &sset, &sset);
	}

}//end of namespace

daemon_t::daemon_t()
{
	this->stop = false;
	this->restart = false;
}

void daemon_t::prase_args( int argc, char** argv )
{
	this->prog_name = argv[0];
	char* dir = ::get_current_dir_name();
	this->current_dir = dir;
	free(dir);

	rlimit_reset();
	set_signal();
	save_argv(argc, argv);
	if (g_bench_conf.get_is_daemon()){
		daemon(1, 1);
	}
}

daemon_t::~daemon_t()
{
	if (this->restart && !this->prog_name.empty()) {
		killall_children();

		ALERT_LOG("SERVER RESTARTING...");
		::chdir(this->current_dir.c_str());
		char* argvs[200];
		int i = 0;
		FOREACH(g_argvs, it){
			argvs[i] = const_cast<char*>(it->c_str());
			i++;
		}

		execv(this->prog_name.c_str(), argvs);
		ALERT_LOG("RESTART FAILED...");
	}
}

void daemon_t::killall_children()
{
	for (uint32_t i = 0; i < g_bind_conf.elems.size(); ++i) {
		if (0 != atomic_read(&this->child_pids[i])) {
			kill(atomic_read(&this->child_pids[i]), SIGKILL);
		}
	}

	/* wait for all child exit*/
WAIT_AGAIN:
	while (1) {
		for (uint32_t i = 0; i < g_bind_conf.elems.size(); ++i) {
			if (0 != atomic_read(&this->child_pids[i])) {
				usleep(100);
				goto WAIT_AGAIN;
			}
		}
		return;
	}
}

void daemon_t::restart_child_process( bind_config_elem_t* elem )
{
	if (g_daemon.stop && !g_daemon.restart){
		//在关闭服务器时,防止子进程先收到信号退出,父进程再次创建子进程.
		return;
	}

	ice::lib_file_t::close_fd(elem->recv_pipe.handles[E_PIPE_INDEX_WRONLY]);
	ice::lib_file_t::close_fd(elem->send_pipe.handles[E_PIPE_INDEX_RDONLY]);

	elem->send_pipe.create();
	elem->recv_pipe.create();

	int i = g_bind_conf.get_elem_idx(elem);
	pid_t pid;

	if ( (pid = fork ()) < 0 ) {
		CRIT_LOG("fork failed: %s", strerror(errno));
	} else if (pid > 0) { 
		//parent process
		ice::lib_file_t::close_fd(g_bind_conf.elems[i].recv_pipe.handles[E_PIPE_INDEX_RDONLY]);
		ice::lib_file_t::close_fd(g_bind_conf.elems[i].send_pipe.handles[E_PIPE_INDEX_WRONLY]);
		g_pipe_fd_elems.insert(std::make_pair(elem->send_pipe.handles[E_PIPE_INDEX_RDONLY], elem));
		g_net_server.get_server_epoll()->add_connect(elem->send_pipe.handles[E_PIPE_INDEX_RDONLY], ice::FD_TYPE_PIPE, NULL);
		atomic_set(&g_daemon.child_pids[i], pid);
	} else { 
		//child process
		g_service.run(&g_bind_conf.elems[i], g_bind_conf.elems.size());
	}
}

int daemon_t::run()
{
	while (likely(!this->stop || 0 != g_dll.functions.on_fini(g_is_parent))) {
		g_net_server.get_server_epoll()->run();
	}
	return 0;
}
