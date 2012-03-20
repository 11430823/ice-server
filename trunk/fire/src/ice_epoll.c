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

ep_info_t g_epi;

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
				bind_config_elem_t* bc = g_epi.bc_elem;
				CRIT_LOG("CHILD PROCESS CRASHED![olid=%u olname=%s]", bc->id, bc->name.c_str());
				char buf[100];
				snprintf(buf, sizeof(buf), "%s.%s", bc->name.c_str(), "child.core");
				//  [9/12/2011 meng]
#if 0
				asynsvr_send_warning(buf, bc->online_id, bc->bind_ip);
#endif
				// close all connections that are related to the crashed child process
				for (int i = 0; i <= g_epi.get_max_fd(); ++i) {
					if ((g_epi.bc_elem == bc) && (g_epi.m_fds[i].fd_type != fd_type_listen)) {
						//todo 
						//do_del_conn(i, g_is_parent);
						//todo end
					}
				}
				// prevent child process from being restarted again and again forever
				if (bc->restart_cnt++ < 20) {
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
				g_daemon.m_stop = true;
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

int ep_info_t::do_add_conn(int fd, uint8_t fd_type, struct sockaddr_in *peer)
{
	static uint32_t seq = 0;
	uint32_t flag;

	switch (fd_type) {
	case fd_type_asyn_connect:
		flag = EPOLLOUT;
		break;
	default:
		flag = EPOLLIN;
		break;
	}

	if (0 != this->add_events(fd, flag)) {
		return -1;
	}

	memset(&this->m_fds[fd], 0, sizeof(struct fdinfo_t));
	this->m_fds[fd].fd = fd;
	this->m_fds[fd].fd_type = fd_type;
	this->m_fds[fd].id = ++seq;
	if (0 == seq) {
		m_fds[fd].id = ++seq;//mark 有可能和前面的ID重复
	}
	if (NULL != peer) {
		this->m_fds[fd].remote_ip = peer->sin_addr.s_addr;
		this->m_fds[fd].remote_port = peer->sin_port;
	}
	this->m_fds[fd].last_tm = ice::get_now_tv()->tv_sec;
	this->max_fd = this->max_fd > fd ? this->max_fd : fd;
	KDEBUG_LOG(0, "time now:%ld, fd:%d, fd type:%d, id%u", ice::get_now_tv()->tv_sec, fd, fd_type, this->m_fds[fd].id);
	return 0;
}

int ep_info_t::loop( int max_len )
{
	return 0;
}

int ep_info_t::init(int maxevents )
{
	if ((this->fd = epoll_create(maxevents)) < 0) {
		ALERT_LOG("EPOLL_CREATE FAILED [error:%s]", strerror (errno));
		return -1;
	}

	m_fds = (struct fdinfo_t*) calloc (maxevents, sizeof (struct fdinfo_t));
	if (NULL == m_fds){
		close (this->fd);
		ALERT_LOG ("CALLOC FDINFO_T FAILED [maxevents=%d]", maxevents);
		return -1;
	}

	this->max_ev_num = maxevents;
	this->max_fd = 0;
	return 0;
}

int ep_info_t::child_loop( int max_len )
{
	epoll_event evs[this->max_ev_num];
	int nr = 0;
	int pos = 0;
	epoll_event ev;
	while ( !g_daemon.m_stop || 0 != g_dll.on_fini(g_is_parent) ) {
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
						do_add_conn(newfd, fd_type_remote, &peer);
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
				if ((g_epi.m_fds[i].fd_type == fd_type_remote)
					&& ((time(0) - (int32_t)g_epi.m_fds[i].last_tm) >= g_bench_conf.get_fd_time_out())) {
						//do_del_conn(i, g_is_parent);
				}
			}
		}

		g_dll.on_events();
	}
	return 0;
}

int ep_info_t::exit()
{
	for (int i = 0; i < this->max_fd; i++) {
		if (this->m_fds[i].fd_type == fd_type_unused){
			continue;
		}
		close (i);
	}

	free (this->m_fds);
	close (this->fd);
	return 0;
}

int ep_info_t::start( const char* listen_ip, in_port_t listen_port, struct bind_config_elem_t* bc_elem )
{
	int ret_code = -1;

	this->bc_elem = bc_elem;

	int listenfd = ice::lib_tcp_sever_t::safe_socket_listen(listen_ip, listen_port, 1024, 32 * 1024);
	if (-1 != listenfd) {
		ice::lib_file_t::set_io_block(listenfd, false);

		do_add_conn(listenfd, fd_type_listen, 0);
		ret_code = 0;
	}

	BOOT_LOG(ret_code, "Listen on %s:%u,ret_code:%d,listenfd:%d",
		listen_ip ? listen_ip : "ANYADDR", listen_port, ret_code, listenfd);
}

int ep_info_t::add_events( int fd, uint32_t flag )
{
	struct epoll_event ev;

	ev.events = flag;
	ev.data.fd = fd;
epoll_add_again:
	if (unlikely (0 != epoll_ctl(this->fd, EPOLL_CTL_ADD, fd, &ev))) {
		switch (errno)
		{
		case EINTR:
			goto epoll_add_again;
			break;
		default:
			ERROR_LOG ("epoll_ctl add fd:%d error:%s", fd, strerror(errno));
			return -1;
			break;
		}
	}
	return 0; 
}
