#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <assert.h>
#include <sys/mman.h>
#include <time.h>
#include <stdint.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

#include <lib_util.h>
#include <lib_log.h>
#include <lib_timer.h>
#include <lib_file.h>
#include <lib_tcp.h>

#include "ice_epoll.h"
#include "service.h"
#include "bind_conf.h"
#include "ice_dll.h"
#include "daemon.h"
#include "bench_conf.h"

net_server_t g_net_server;

enum {
	trash_size		= 4096,
	mcast_pkg_size	= 8192,
	udp_pkg_size	= 8192
};

namespace {
		//************************************
	// Brief:     处理管道事件(父/子进程crashed)
	// Returns:   int
	// Parameter: int fd
	// Parameter: int pos
	//************************************
	int handle_pipe_event(int fd, epoll_event& r_evs)
	{
		if (r_evs.events & EPOLLHUP) {
			if (g_is_parent) { // Child Crashed
				//int pfd = r_evs.data.fd;
				bind_config_elem_t* bc = g_net_server.bc_elem;
				CRIT_LOG("CHILD PROCESS CRASHED![olid=%u olname=%s]", bc->id, bc->name.c_str());
				char buf[100];
				snprintf(buf, sizeof(buf), "%s.%s", bc->name.c_str(), "child.core");
				//  [9/12/2011 meng]
#if 0
				asynsvr_send_warning(buf, bc->online_id, bc->bind_ip);
#endif
				// close all connections that are related to the crashed child process
				for (int i = 0; i <= g_net_server.get_max_fd(); ++i) {
					if ((g_net_server.bc_elem == bc) && (g_net_server.m_fds[i].fd_type != fd_type_listen)) {
						//todo 
						//do_del_conn(i, g_is_parent);
						//todo end
					}
				}
				// prevent child process from being restarted again and again forever
				if (bc->restart_cnt++ <= g_bench_conf.get_restart_cnt_max()) {
					g_daemon.restart_child_process(bc);
				}
			} else { // Parent Crashed
				CRIT_LOG("PARENT PROCESS CRASHED!");

				char buf[100];
				snprintf(buf, sizeof(buf), "%s.%s", g_service.m_bind_elem->get_name().c_str(), "parent.core");
				//  [9/12/2011 meng]	
#if 0
				asynsvr_send_warning(buf, 0, get_server_ip());
#endif
				g_daemon.stop = true;
				return -1;
			}
		} else {
			char trash[trash_size];
			while (trash_size == read(fd, trash, trash_size)) ;
		}

		return 0;
	}

}//end namespace

int mod_events(int epfd, int fd, uint32_t flag)
{
	struct epoll_event ev;

	ev.events = EPOLLET | flag;
	ev.data.fd = fd;

epoll_mod_again:
	if (unlikely (epoll_ctl (epfd, EPOLL_CTL_MOD, fd, &ev) != 0)) {
		//		ERROR_LOG ("epoll_ctl mod %d error: %m", fd);
		if (errno == EINTR)
			goto epoll_mod_again;
		return -1;
	}

	return 0;
}

int net_server_t::create(uint32_t max_fd_num)
{
	this->server_epoll = new ice::lib_tcp_server_epoll_t(max_fd_num);
	if (NULL == this->server_epoll){
		ALERT_LOG("new tcp_server_epoll err [maxevents:%u]", max_fd_num);
		return -1;
	}
	int ret = 0;
	if (0 != (ret = this->server_epoll->create())){
		ALERT_LOG("new tcp_server_epoll create err [ret:%d]", ret);
		return -1;
	}

	return 0;
}

int net_server_t::destroy()
{
	ice::safe_delete(this->server_epoll);
	return 0;
}

int net_server_t::listen( const char* listen_ip, in_port_t listen_port, struct bind_config_elem_t* bc_elem )
{
	int ret_code = -1;

	this->bc_elem = bc_elem;

	int listenfd = ice::lib_tcp_sever_t::safe_socket_listen(listen_ip, listen_port, 1024, 32 * 1024);
	if (-1 != listenfd) {
		ice::lib_file_t::set_io_block(listenfd, false);

		this->server_epoll->add_connect(listenfd, fd_type_listen, 0);
		ret_code = 0;
	}

	BOOT_LOG(ret_code, "Listen on %s:%u,ret_code:%d,listenfd:%d",
		listen_ip ? listen_ip : "ANYADDR", listen_port, ret_code, listenfd);
}

int net_server_t::daemon_run()
{
	while (!g_daemon.stop || g_dll.on_fini(g_is_parent) != 0) {
		g_net_server.get_server_epoll()->run();
	}
}

int net_server_t::service_run()
{
	int max_len = 0;
		epoll_event evs[this->max_ev_num];
	int nr = 0;
	int pos = 0;
	epoll_event ev;
	while ( !g_daemon.stop || 0 != g_dll.on_fini(g_is_parent) ) {
		nr = epoll_wait(this->fd , evs, this->max_ev_num, EPOLL_TIME_OUT);

		if (unlikely(-1 == nr && errno != EINTR)){
			ALERT_LOG("EPOLL_WAIT FAILED, [maxfd=%d, epfd=%d, error:%s]",
				this->max_fd, this->fd, strerror(errno));
			return -1;
		}

		ice::renew_now();

		for (pos = 0; pos < nr; pos++) {
			int fd = evs[pos].data.fd;
			fdinfo_t& fdinfo = this->m_fds[fd];
			if (fd > this->max_fd || fdinfo.fd != fd || fdinfo.fd_type == fd_type_unused) {
				ERROR_LOG("DELAYED EPOLL EVENTS [event fd=%d, cache fd=%d, maxfd=%d, type=%d]", 
					fd, fdinfo.fd, this->max_fd, fdinfo.fd_type);
				continue;
			}

			if ( unlikely(fd_type_pipe == fdinfo.fd_type ) ) {
				if (0 == handle_pipe_event(fd, evs[pos])) {
					continue;
				} else {
					return -1;
				}
			}

			if (evs[pos].events & EPOLLIN) {
				switch (fdinfo.fd_type) {
				case fd_type_listen:
					//accept
					//////////////////////////////////////////////////////////////////////////
					struct sockaddr_in peer;
					int newfd;

					newfd = ice::lib_tcp_sever_t::safe_tcp_accept(fd, &peer, false);
					if (-1 != newfd) {
						get_server_epoll()->add_connect(newfd, fd_type_remote, &peer);
						//todo 后面的错误处理
					} else if ((errno == EMFILE) || (errno == ENFILE)) {
						//add_to_etin_queue(fd);
					} else if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
						//del_from_etin_queue(fd);
					}else{
						ERROR_LOG("ACCEPT ERROR:%s", strerror(errno));
					}
					break;
				case fd_type_remote:
					{
						char buf[1500 + 1] = {0};
						int len;
						/* 开始处理每个新连接上的数据收发 */
						/* 接收客户端的消息 */
						len = recv(fd, buf, sizeof(buf)-1, 0);
						switch(len)
						{
    						case 0:
								{
									//todo 删除
									int e = epoll_ctl(this->fd, EPOLL_CTL_DEL, fd, &ev);
									if(-1 == e)
									{
										//m_ilog.err("file:%s,line:%d,errno:%d,%s",__FILE__,__LINE__,errno,strerror(errno));
									}
									close(fd);
								}
    						break;
    						case -1:
								if(EAGAIN != errno){// 断开
									//m_ilog.err("file:%s,line:%d,errno:%d,%s",__FILE__,__LINE__,errno,strerror(errno));
									perror("11 != errno:");
									epoll_ctl(this->fd, EPOLL_CTL_DEL, fd, &ev);
								}	
    							break;
    						default:
    							{
    								buf[len] = '\0';
    								std::cout << buf << std::endl;
										int i = send(fd, buf, len, 0);
										std::cout << "send : " << i << std::endl;
								}
    							break;
						}
					}
					break;
				default:
					ALERT_LOG("FDINFO TYPE[TYPE=%u]", fdinfo.fd_type);
					break;
				}
			}

			if (evs[pos].events & EPOLLOUT) {
				//该套接字可写
				/*
				if (fdinfo.cb.sendlen > 0 && do_write_conn(fd) == -1) {
					//do_del_conn(fd, g_is_parent);
				}
				if (fdinfo.cb.sendlen == 0) {
					mod_events(this->fd, fd, EPOLLIN);
				}
				}*/
			}

			if (evs[pos].events & EPOLLHUP) {
				//do_del_conn(fd, g_is_parent);
			}
		}

		if (g_bench_conf.get_fd_time_out()) {
			for (int i = 0; i <= this->max_fd; ++i) {
				if ((g_net_server.m_fds[i].fd_type == fd_type_remote)
					&& ((time(0) - (int32_t)g_net_server.m_fds[i].last_tm) >= g_bench_conf.get_fd_time_out())) {
						//do_del_conn(i, g_is_parent);
				}
			}
		}

		g_dll.on_events();
	}
	return 0;
}
