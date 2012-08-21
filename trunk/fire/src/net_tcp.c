#include <lib_include.h>

#include <lib_util.h>
#include <lib_log.h>
#include <lib_linux_version.h>
#include <lib_timer.h>
#include <lib_time.h>

#include "daemon.h"
#include "service.h"
#include "bind_conf.h"
#include "net_tcp.h"
#include "bench_conf.h"

net_server_t g_net_server;

namespace {

	const uint32_t RECV_BUF_LEN = 1024*8;//8K
	/**
	 * @brief	接收对方消息
	 * @param	ice::lib_tcp_client_t & cli_info
	 * @return	int 0:断开 >0:接收的数据长度
	 */
	int recv_peer_msg(ice::lib_tcp_peer_info_t& cli_info)
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

}//end namespace 

int net_server_t::create(uint32_t max_fd_num)
{
	this->server_epoll = new tcp_server_epoll_t(max_fd_num, g_bench_conf.get_fd_time_out());
	if (NULL == this->server_epoll){
		ALERT_LOG("new tcp_server_epoll err [maxevents:%u]", max_fd_num);
		return -1;
	}
	int ret = 0;
	if (0 != (ret = this->server_epoll->create())){
		ALERT_LOG("new tcp_server_epoll create err [ret:%d]", ret);
		this->destroy();
		return -1;
	}

	return 0;
}

int net_server_t::destroy()
{
	SAFE_DELETE(this->server_epoll);
	return 0;
}

net_server_t::net_server_t( void )
{
	this->server_epoll = NULL;
}

int tcp_server_epoll_t::run( CHECK_RUN check_run_fn )
{
	int event_num = 0;//事件的数量
	epoll_event evs[this->cli_fd_value_max];
	while(check_run_fn()){
		event_num = HANDLE_EINTR(::epoll_wait(this->fd, evs, this->cli_fd_value_max, this->epoll_wait_time_out));
		ice::lib_timer_t::renew_now();
		if (!g_is_parent){
			g_addr_mcast.syn_info();
		}
		this->on_functions->on_events();
		if (0 == event_num){
			//time out
			continue;
		}else if (unlikely(-1 == event_num)){
			ALERT_LOG("epoll wait err [%s]", ::strerror(errno));
			return -1;
		}

		//handling event
		for (int i = 0; i < event_num; ++i){
			ice::lib_tcp_peer_info_t& fd_info = this->peer_fd_infos[evs[i].data.fd];
			if ( unlikely(ice::FD_TYPE_PIPE == fd_info.get_fd_type()) ) {
				if (0 == this->on_pipe_event(fd_info.get_fd(), evs[i])) {
					continue;
				} else {
					return -1;
				}
			}
			//可读或可写(可同时发生,并不互斥)
			if(EPOLLOUT & evs[i].events){//该套接字可写
				if (this->handle_send(fd_info) < 0){
					this->close_peer(fd_info);
					continue;
				}
			}
			if(EPOLLIN & evs[i].events){//接收并处理其他套接字的数据
				if (ice::FD_TYPE_LISTEN == fd_info.get_fd_type()){
					this->handle_listen();
					continue;
				} else if (ice::FD_TYPE_CLI == fd_info.get_fd_type()){
					this->handle_peer_msg(fd_info);			
				} else if (ice::FD_TYPE_SVR == fd_info.get_fd_type()){
					this->handle_peer_msg(fd_info);	
				} else if (ice::FD_TYPE_MCAST == fd_info.get_fd_type()){
					this->handle_peer_mcast_msg(fd_info);
				} else if (ice::FD_TYPE_ADDR_MCAST == fd_info.get_fd_type()){
					this->handle_peer_add_mcast_msg(fd_info);
				}
				
			}
			if (EPOLLHUP & evs[i].events){
				// EPOLLRDHUP: If a client send some data to a server and than close the connection
				//					  immediately, the server will receive RDHUP and IN at the same time.
				//					  Under ET mode, this can make a sockfd into CLOSE_WAIT state.
				// EPOLLHUP: When close of a fd is detected (ie, after receiving a RST segment:
				//				   the client has closed the socket, and the server has performed
				//				   one write on the closed socket.)
				// After receiving EPOLLRDHUP or EPOLLHUP, we can still read data from the fd until
				// read() returns 0 indicating EOF is reached. So, we should alway call read on receving
				// this kind of events to aquire the remaining data and/or EOF. (Linux-2.6.18)
				//todo 可读
			}
			if(EPOLLERR & evs[i].events){
					//收到这个//socket仍可读数据?

					/************************************************************************/
					/*只有采取动作时，才能知道是否对方异常。即对方突然断掉，是不可能
					有此事件发生的。只有自己采取动作（当然自己此刻也不知道），read，
					write时，出EPOLLERR错，说明对方已经异常断开。

					EPOLLERR 是服务器这边出错（自己出错当然能检测到，对方出错你咋能
					直到啊）*/
					/************************************************************************/
					//disconnect(handle_fd);
			}
			if(EPOLLRDHUP & evs[i].events){
					//对端close
					/************************************************************************/
					/*	  　　EPOLLRDHUP = 0x2000,
					　　#define EPOLLRDHUP EPOLLRDHUP*/
					/************************************************************************/
					//disconnect(handle_fd);
			}
			if(!(evs[i].events & EPOLLOUT) && !(evs[i].events & EPOLLIN)){
				ERROR_LOG("events:%u", evs[i].events);
			}
		}
	}
	return 0;
}

tcp_server_epoll_t::tcp_server_epoll_t(uint32_t max_events_num, uint32_t cli_time_out)
{
	this->cli_fd_value_max = max_events_num;
	this->epoll_wait_time_out = -1;
	this->on_pipe_event = NULL;
	this->on_functions = NULL;
	this->cli_time_out_sec = cli_time_out;
}

int tcp_server_epoll_t::listen(const char* ip, uint16_t port, uint32_t listen_num, int bufsize)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)
	this->listen_fd = ::socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
#else
	this->listen_fd = ::socket(PF_INET, SOCK_STREAM, 0);
#endif
	if (INVALID_FD == this->listen_fd){
		ALERT_LOG("create socket err [%s]", ::strerror(errno));
		return -1;
	}

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27)
	ice::lib_file_t::set_io_block(this->listen_fd, false);
#endif

	int ret = ice::lib_tcp_t::set_reuse_addr(this->listen_fd);
	if (-1 == ret){
		ice::lib_file_t::close_fd(this->listen_fd);
		return -1;

	}
	ret = ice::lib_net_util_t::set_recvbuf(this->listen_fd, bufsize);
	if (-1 == ret){
		ice::lib_file_t::close_fd(this->listen_fd);
		return -1;
	}
	ret = ice::lib_net_util_t::set_sendbuf(this->listen_fd, bufsize);
	if(-1 == ret){
		ice::lib_file_t::close_fd(this->listen_fd);
		return -1;
	}

	if (0 != this->bind(ip, port)){
		ice::lib_file_t::close_fd(this->listen_fd);
		ALERT_LOG("bind socket err [%s]", ::strerror(errno));
		return -1;
	}

	if (0 != ::listen(this->listen_fd, listen_num)){
		ice::lib_file_t::close_fd(this->listen_fd);
		ALERT_LOG("listen err [%s]", ::strerror(errno));
		return -1;
	}

	if (NULL == this->add_connect(this->listen_fd, ice::FD_TYPE_LISTEN, ip, port)){
		ice::lib_file_t::close_fd(this->listen_fd);
		ALERT_LOG("add connect err[%s]", ::strerror(errno));
		return -1;
	}

	this->set_ip(ice::lib_net_util_t::ip2int(ip));
	this->set_port(port);

	return 0;
}

int tcp_server_epoll_t::create()
{
	if ((this->fd = ::epoll_create(this->cli_fd_value_max)) < 0) {
		ALERT_LOG("EPOLL_CREATE FAILED [ERROR:%s]", strerror (errno));
		return -1;
	}

	this->peer_fd_infos = (ice::lib_tcp_peer_info_t*) new ice::lib_tcp_peer_info_t[this->cli_fd_value_max];
	if (NULL == this->peer_fd_infos){
		ice::lib_file_t::close_fd(this->fd);
		ALERT_LOG ("CALLOC CLI_FD_INFO_T FAILED [MAXEVENTS=%d]", this->cli_fd_value_max);
		return -1;
	}
	return 0;
}

int tcp_server_epoll_t::add_events( int fd, uint32_t flag )
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

ice::lib_tcp_peer_info_t* tcp_server_epoll_t::add_connect( int fd, ice::E_FD_TYPE fd_type, const char* ip, uint16_t port )
{
	uint32_t flag;

	switch (fd_type) {
	default:
		flag = EPOLLIN;
		break;
	}

	if (0 != this->add_events(fd, flag)) {
		ERROR_LOG("add events err [fd:%d, flag:%u]", fd, flag);
		return NULL;
	}

	ice::lib_tcp_peer_info_t& cfi = this->peer_fd_infos[fd];
	cfi.init();
	cfi.set_fd(fd);
	cfi.set_fd_type(fd_type);
	cfi.set_last_tm(ice::lib_time_t::get_now_second());
	if (NULL != ip) {
		cfi.set_ip(ice::lib_net_util_t::ip2int(ip));
		cfi.set_port(port);
	}

	TRACE_LOG("time now:%u, fd:%d, fd type:%d, ip:%s, port:%u", 
		cfi.get_last_tm(), fd, fd_type, cfi.get_ip_str().c_str(), cfi.get_port());
	return &cfi;
}

void tcp_server_epoll_t::handle_peer_msg( ice::lib_tcp_peer_info_t& fd_info )
{
	int ret = recv_peer_msg(fd_info);
	if (ret > 0){
		int available_len = 0;
		while (0 != (available_len = this->on_functions->on_get_pkg_len(&fd_info, fd_info.recv_buf.get_data(), fd_info.recv_buf.get_write_pos()))){	
			if (-1 == available_len){
				this->close_peer(fd_info);
				break;
			}else if (available_len > 0 && (int)fd_info.recv_buf.get_write_pos() >= available_len){
				if (ice::FD_TYPE_CLI == fd_info.get_fd_type()){
					if (0 != this->on_functions->on_cli_pkg(fd_info.recv_buf.get_data(), available_len, &fd_info)){
						this->close_peer(fd_info);
					}
				} else if (ice::FD_TYPE_SVR == fd_info.get_fd_type()){
					this->on_functions->on_srv_pkg(fd_info.recv_buf.get_data(), available_len, &fd_info);
				}
				fd_info.recv_buf.pop_front(available_len);
			}else{
				break;
			}
		}
	}else if (0 == ret || -1 == ret){
		ERROR_LOG("close socket by peer [fd:%d, ip:%s, port:%u]", fd_info.get_fd(), fd_info.get_ip_str().c_str(), fd_info.get_port());
		this->close_peer(fd_info);
	}
}

void tcp_server_epoll_t::handle_listen()
{
	int peer_fd = -1;
	sockaddr_in peer;
	memset(&peer,0,sizeof(peer));
	peer_fd = this->accept(peer);
	if (unlikely(peer_fd < 0)){
		ALERT_LOG("accept err [%s]", ::strerror(errno));
	}else{
		TRACE_LOG("client accept [ip:%s, port:%u, new_socket:%d]",
			inet_ntoa(peer.sin_addr), ntohs(peer.sin_port), peer_fd);
		this->add_connect(peer_fd, ice::FD_TYPE_CLI, ice::lib_net_util_t::ip2str(peer.sin_addr.s_addr).c_str(), ntohs(peer.sin_port));
	}
}

int tcp_server_epoll_t::handle_send( ice::lib_tcp_peer_info_t& fd_info )
{
	int send_len = fd_info.send(fd_info.send_buf.get_data(), fd_info.send_buf.get_write_pos());
	if (send_len > 0){
		uint32_t remain_len = fd_info.send_buf.pop_front(send_len);
		if (0 == remain_len){
			this->mod_events(fd_info.get_fd(), EPOLLIN | EPOLLRDHUP);//todo ? EPOLLRDHUP
		}
	}
	return send_len;
}

int tcp_server_epoll_t::mod_events( int fd, uint32_t flag )
{
	epoll_event ev;
	ev.events = flag;
	ev.data.fd = fd;

	int ret = HANDLE_EINTR(::epoll_ctl(this->fd, EPOLL_CTL_MOD, fd, &ev));
	if (0 != ret){
		ERROR_LOG ("epoll_ctl mod fd:%d error:%s", fd, strerror(errno));
		return -1;
	}
	return 0; 
}

tcp_server_epoll_t::~tcp_server_epoll_t()
{

}

void tcp_server_epoll_t::register_on_functions( const ice::on_functions_tcp_srv* functions )
{
	this->on_functions = (on_functions_tcp_server_epoll*)functions;
}

void tcp_server_epoll_t::close_peer( ice::lib_tcp_peer_info_t& fd_info, bool do_calback /*= true*/ )
{
	if (do_calback){
		if (ice::FD_TYPE_CLI == fd_info.get_fd_type()){
			ERROR_LOG("close socket cli [fd:%d, ip:%s, port:%u]", fd_info.get_fd(), fd_info.get_ip_str().c_str(), fd_info.get_port());
			this->on_functions->on_cli_conn_closed(fd_info.get_fd());
		} else if (ice::FD_TYPE_SVR == fd_info.get_fd_type()){
			ERROR_LOG("close socket svr [fd:%d, ip:%s, port:%u]", fd_info.get_fd(), fd_info.get_ip_str().c_str(), fd_info.get_port());
			this->on_functions->on_svr_conn_closed(fd_info.get_fd());
		}
	}
	fd_info.close();
}

void tcp_server_epoll_t::handle_peer_mcast_msg( ice::lib_tcp_peer_info_t& fd_info )
{
	recv_peer_msg(fd_info);
	if (fd_info.recv_buf.get_write_pos() > 0){
		this->on_functions->on_mcast_pkg(fd_info.recv_buf.get_data(), fd_info.recv_buf.get_write_pos());
		fd_info.recv_buf.pop_front(fd_info.recv_buf.get_write_pos());
	}
}

void tcp_server_epoll_t::handle_peer_add_mcast_msg( ice::lib_tcp_peer_info_t& fd_info )
{
	recv_peer_msg(fd_info);
	if (fd_info.recv_buf.get_write_pos() > 0){
		g_addr_mcast.handle_msg(fd_info.recv_buf);
		fd_info.recv_buf.pop_front(fd_info.recv_buf.get_write_pos());
	}
}