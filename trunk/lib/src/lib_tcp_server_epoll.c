#include <sys/epoll.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>

#include "lib_log.h"
#include "lib_file.h"
#include "lib_time.h"
#include "lib_timer.h"
#include "lib_linux_version.h"
#include "lib_tcp_server_epoll.h"
#include "lib_tcp_client.h"

const uint32_t RECV_BUF_LEN = 1024*8;//8K

namespace {
	/**
	 * @brief	接收客户端消息
	 * @param	ice::lib_tcp_client_t & cli_info
	 * @return	int 0:断开 >0:接收的数据长度
	 */
	int client_recv(ice::lib_tcp_client_t& cli_info)
	{
		char buf[RECV_BUF_LEN] = {0};
		int len = 0;

		int sum_len = 0;
		while (1) {
			len = cli_info.recv(buf, sizeof(buf));
			if (likely(len > 0)){
				if (len < (int)sizeof(buf)){
					cli_info.recv_buf.push_back(buf, len);
					return len;
				}else if (sizeof(buf) == len){
					cli_info.recv_buf.push_back(buf, len);
					sum_len += len;
					continue;
				}
			} else if (0 == len){
				return 0;
			} else if (len == -1) {
				return sum_len;
			}
		}
		return sum_len;
	}

	int server_recv()
	{
		//todo
		return 0;
	}

}//end namespace 


int ice::lib_tcp_server_epoll_t::run( CHECK_RUN check_run_fn )
{
	int event_num = 0;//事件的数量
	epoll_event evs[this->cli_fd_value_max];

	while(check_run_fn()){
		event_num = HANDLE_EINTR(::epoll_wait(this->fd, evs, this->cli_fd_value_max, this->epoll_wait_time_out));
		renew_now();
		//todo 添加事件处理机制接口.
		if (0 == event_num){
			//time out
			continue;
		}else if (unlikely(-1 == event_num)){
			ALERT_LOG("epoll wait err [%s]", ::strerror(errno));
			return -1;
		}

		//handling event
		for (int i = 0; i < event_num; ++i){
			lib_tcp_client_t& fd_info = this->cli_fd_infos[evs[i].data.fd];
			if ( unlikely(FD_TYPE_PIPE == fd_info.fd_type) ) {
				if (0 == this->on_pipe_event(fd_info.get_fd(), evs[i])) {
					continue;
				} else {
					return -1;
				}
			}
			//可读或可写(可同时发生,并不互斥)
			if(EPOLLOUT & evs[i].events){//该套接字可写
				this->handle_send(fd_info);
			}
			if(EPOLLIN & evs[i].events){//接收并处理其他套接字的数据
				if (FD_TYPE_LISTEN == fd_info.fd_type){
					this->handle_listen();
					continue;
				} else if (FD_TYPE_CLIENT == fd_info.fd_type){
					this->handle_client(fd_info);			
				}else if (FD_TYPE_SERVER == fd_info.fd_type){
				}
			}
		}
	}
	return 0;
}

ice::lib_tcp_server_epoll_t::lib_tcp_server_epoll_t(uint32_t max_events_num)
{
	this->cli_fd_value_max = max_events_num;
	this->epoll_wait_time_out = -1;
	this->on_pipe_event = NULL;
	this->on_functions = NULL;
}

int ice::lib_tcp_server_epoll_t::listen(const char* ip, uint16_t port, uint32_t listen_num, int bufsize)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)
	this->listen_fd = ::socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
#else
	this->listen_fd = ::socket(PF_INET, SOCK_STREAM, 0);
#endif
	if (-1 == this->listen_fd){
		ALERT_LOG("create socket err [%s]", ::strerror(errno));
		return -1;
	}

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27)
	lib_file_t::set_io_block(this->listen_fd, false);
#endif

	int ret = lib_tcp_t::set_reuse_addr(this->listen_fd);
	if (-1 == ret){
		lib_file_t::close_fd(this->listen_fd);
		return -1;

	}
	ret = set_recvbuf(this->listen_fd, bufsize);
	if (-1 == ret){
		lib_file_t::close_fd(this->listen_fd);
		return -1;
	}
	ret = lib_tcp_t::set_sendbuf(this->listen_fd, bufsize);
	if(-1 == ret){
		lib_file_t::close_fd(this->listen_fd);
		return -1;
	}

	if (0 != this->bind(ip, port)){
		lib_file_t::close_fd(this->listen_fd);
		ALERT_LOG("bind socket err [%s]", ::strerror(errno));
		return -1;
	}

	if (0 != ::listen(this->listen_fd, listen_num)){
		lib_file_t::close_fd(this->listen_fd);
		ALERT_LOG("listen err [%s]", ::strerror(errno));
		return -1;
	}

	sockaddr_in sa_in;
	memset(&sa_in, 0, sizeof(sa_in));
	sa_in.sin_addr.s_addr = inet_addr(ip);
	sa_in.sin_family = PF_INET;
	sa_in.sin_port = htons(port);
	if (0 != this->add_connect(this->listen_fd, FD_TYPE_LISTEN, &sa_in)){
		lib_file_t::close_fd(this->listen_fd);
		ALERT_LOG("add connect err[%s]", ::strerror(errno));
		return -1;
	}

	return 0;
}

int ice::lib_tcp_server_epoll_t::create()
{
	if ((this->fd = epoll_create(this->cli_fd_value_max)) < 0) {
		ALERT_LOG("EPOLL_CREATE FAILED [ERROR:%s]", strerror (errno));
		return -1;
	}

	this->cli_fd_infos = (struct lib_tcp_client_t*) new lib_tcp_client_t[this->cli_fd_value_max];
	if (NULL == this->cli_fd_infos){
		lib_file_t::close_fd(this->fd);
		ALERT_LOG ("CALLOC CLI_FD_INFO_T FAILED [MAXEVENTS=%d]", this->cli_fd_value_max);
		return -1;
	}
	return 0;
}

int ice::lib_tcp_server_epoll_t::add_events( int fd, uint32_t flag )
{
	epoll_event ev;
	ev.events = flag;
	ev.data.fd = fd;

	int ret = HANDLE_EINTR(::epoll_ctl(this->fd, EPOLL_CTL_ADD, fd, &ev));
	if (0 != ret){
		ERROR_LOG ("epoll_ctl add fd:%d error:%s", fd, strerror(errno));
		return -1;
	}

	return 0; 
}

int ice::lib_tcp_server_epoll_t::add_connect( int fd, E_FD_TYPE fd_type, struct sockaddr_in* peer )
{
	uint32_t flag;

	switch (fd_type) {
	default:
		flag = EPOLLIN;
		break;
	}

	if (0 != this->add_events(fd, flag)) {
		ERROR_LOG("add events err [fd:%d, flag:%u]", fd, flag);
		return -1;
	}

	lib_tcp_client_t& cfi = this->cli_fd_infos[fd];
	cfi.init();
	cfi.set_fd(fd);
	cfi.fd_type = fd_type;
	cfi.last_tm = ice::lib_time_t::get_now_second();
	if (NULL != peer) {
		cfi.remote_ip = peer->sin_addr.s_addr;
		cfi.remote_port = peer->sin_port;
	}

	TRACE_LOG("time now:%u, fd:%d, fd type:%d", cfi.last_tm, fd, fd_type);
	return 0;
}

void ice::lib_tcp_server_epoll_t::register_pipe_event_fn( ON_PIPE_EVENT fn )
{
	this->on_pipe_event = fn;
}

void ice::lib_tcp_server_epoll_t::handle_client( lib_tcp_client_t& fd_info )
{
	int ret = client_recv(fd_info);
	if (ret > 0){
		int available_len = 0;
		while (0 != (available_len = this->on_functions->on_get_pkg_len(&fd_info, fd_info.recv_buf.get_data(), fd_info.recv_buf.get_write_pos()))){	
			if (-1 == available_len){
				this->on_functions->on_cli_conn_closed(fd_info.get_fd());
				fd_info.close();
				break;
			}else if (available_len > 0 && (int)fd_info.recv_buf.get_write_pos() >= available_len){
				this->on_functions->on_cli_pkg(fd_info.recv_buf.get_data(), available_len, &fd_info);
				fd_info.recv_buf.pop_front(available_len);
			}else{
				break;
			}
		}
	}else if (0 == ret || -1 == ret){
		this->on_functions->on_cli_conn_closed(fd_info.get_fd());
		ERROR_LOG("close socket by peer [fd:%d, ip:%s, port:%u]", fd_info.get_fd(), fd_info.get_ip(), fd_info.remote_port);
		fd_info.close();
	}
}

void ice::lib_tcp_server_epoll_t::handle_listen()
{
	int accept_fd = -1;//连接上的客户SOCKET
	sockaddr_in peer;//对方sockaddr_in
	memset(&peer,0,sizeof(peer));
	accept_fd = this->accept(peer, false);
	if (unlikely(accept_fd < 0)){
		ALERT_LOG("accept err [%s]", ::strerror(errno));
	}else{
		TRACE_LOG("client accept [ip:%s, port:%u, new_socket:%d]",
			inet_ntoa(peer.sin_addr), ntohs(peer.sin_port), accept_fd);
		if (0 != this->add_connect(accept_fd, ice::FD_TYPE_CLIENT, &peer)){
		}
	}
}

void ice::lib_tcp_server_epoll_t::handle_send( lib_tcp_client_t& fd_info )
{
	int send_len = this->send(fd_info.send_buf.get_data(), fd_info.send_buf.get_write_pos());
	if (send_len > 0){
		uint32_t remain_len = fd_info.send_buf.pop_front(send_len);
		if (0 == remain_len){
			//todo 从epoll中移除
		}
	} else if (send_len < 0){
		this->on_functions->on_cli_conn_closed(fd_info.get_fd());
		fd_info.close();
	}
}

int service_run()
{
#if 0
	enum {
		trash_size		= 4096,
		mcast_pkg_size	= 8192,
		udp_pkg_size	= 8192
	};
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
#endif
	return 0;
}
