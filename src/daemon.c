#include <stddef.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>

#include "daemon.h"
#include "bind_conf.h"
#include "log.h"
#include "bench_conf.h"
#include "service.h"
#include "util.h"

daemon_info_t g_daemon;

namespace {

	std::vector<std::string> g_argvs;
	const char version[] = "0.0.1";
	const int core_size = 1 << 30;

	void sigterm_handler(int signo) 
	{
		ALERT_LOG("SIG_TERM FROM [pid=%d, is_parent:%d]", getpid(), is_parent);
		g_daemon.stop     = true;
		g_daemon.restart  = false;
	}

	void sighup_handler(int signo) 
	{
		ALERT_LOG("SIGHUP FROM [pid=%d]", getpid());
		g_daemon.restart  = true;
		g_daemon.stop     = true;
	}

	void sigchld_handler(int signo, siginfo_t *si, void * p) 
	{
		ALERT_LOG("SIGCHLD FROM [pid=%d, is_parent:%d]", getpid(), is_parent);
		pid_t pid;
		static int	status;
		while ((pid = waitpid (-1, &status, WNOHANG)) > 0) {
			for (uint32_t i = 0; i < g_bind_conf.get_elem_num(); ++i) {
				if (atomic_read(&g_daemon.child_pids[i]) == pid) {
					atomic_set(&g_daemon.child_pids[i], 0);
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
		rlim.rlim_cur = core_size;
		rlim.rlim_max = core_size;
		if (-1 == setrlimit(RLIMIT_CORE, &rlim)) {
			ALERT_LOG("INIT CORE FILE RESOURCE FAILED [CORE DUMP SIZE:%d]", core_size);
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

daemon_info_t::daemon_info_t()
{
	stop = false;
	restart = false;
}

void daemon_info_t::prase_args( int argc, char** argv )
{
	prog_name = argv[0];
	char* dir = get_current_dir_name();
	current_dir = dir;
	free(dir);

	rlimit_reset();
	set_signal();
	save_argv(argc, argv);
	if (g_bench_conf.is_daemon()){
		daemon (1, 1);
	}
}

daemon_info_t::~daemon_info_t()
{
	if (g_daemon.restart && !g_daemon.prog_name.empty() && g_argvs.size() > 0) {
		ALERT_LOG("%s", "SERVER RESTARTING...");
		chdir(g_daemon.current_dir.c_str());
		char* argvs[200];
		int i = 0;
		FOREACH(g_argvs, it){
			argvs[i] = const_cast<char*>(it->c_str());
			i++;
		}

		execv(g_daemon.prog_name.c_str(), argvs);
		ALERT_LOG("%s", "RESTART FAILED...");
	}
}

void daemon_info_t::killall_children()
{
	for (uint32_t i = 0; i < g_bind_conf.get_elem_num(); ++i) {
		if (0 != atomic_read(&child_pids[i])) {
			kill(atomic_read(&child_pids[i]), SIGKILL);
		}
	}

	/* wait for all child exit*/
WAIT_AGAIN:
	while (1) {
		for (uint32_t i = 0; i < g_bind_conf.get_elem_num(); ++i) {
			if (0 != atomic_read(&child_pids[i])) {
				usleep(100);
				goto WAIT_AGAIN;
			}
		}
		return;
	}
}