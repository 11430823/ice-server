
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <assert.h>
#include <netdb.h>

#include "lib_file.h"
#include "lib_net_util.h"
#include "lib_tcp.h"

int ice::lib_tcp_t::set_sock_send_timeo( int sockfd, int millisec )
{
	struct timeval tv;

	tv.tv_sec  = millisec / 1000;
	tv.tv_usec = (millisec % 1000) * 1000;

	return setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
}

int ice::lib_tcp_t::set_sock_rcv_timeo( int sockfd, int millisec )
{
	struct timeval tv;

	tv.tv_sec  = millisec / 1000;
	tv.tv_usec = (millisec % 1000) * 1000;

	return setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
}

int ice::lib_tcp_t::safe_tcp_send_n( int sockfd, const void* buf, int total )
{
	int send_bytes = 0;
	for (int cur_len = 0; send_bytes < total; send_bytes += cur_len) {
		//send函数最后一个参数,windows下一般设置为0,linux下最好设置为MSG_NOSIGNAL，
		//如果不设置，在发送出错后有可 能会导致程序退出。
		cur_len = send(sockfd, (char*)buf + send_bytes, total - send_bytes, MSG_NOSIGNAL);
		if (-1 == cur_len) {
			if (errno == EINTR) {
				cur_len = 0;
			} else if (errno == EAGAIN) {
				break;
			} else {
				return -1;
			}
		}
	}

	return send_bytes;
}

int ice::lib_tcp_t::safe_tcp_recv( int sockfd, void* buf, int bufsize )
{
	int cur_len = 0;
	while(1){
		cur_len = recv(sockfd, buf, bufsize, 0);
		if (-1 == cur_len && errno == EINTR) {
			continue;
		}else{
			break;
		}
	}
	return cur_len;
}

int ice::lib_tcp_t::safe_tcp_recv_n( int sockfd, void* buf, int total )
{
	int recv_bytes = 0;

	for (int cur_len = 0; recv_bytes < total; recv_bytes += cur_len)	{
		cur_len = recv(sockfd, (char*)buf + recv_bytes, total - recv_bytes, 0);
		if (0 == cur_len) {
			// connection closed by client
			return 0;
		} else if (-1 == cur_len) {
			if (errno == EINTR) {
				cur_len = 0;
			} else if (errno == EAGAIN)	{
				return recv_bytes;
			} else {
				return -1;
			}
		}
	}

	return recv_bytes;
}

int ice::lib_tcp_t::set_recvbuf( int s, uint32_t len )
{
	return  setsockopt(s, SOL_SOCKET, SO_RCVBUF, &len, sizeof(len));
}

int ice::lib_tcp_t::set_sendbuf( int s, uint32_t len )
{
	return setsockopt(s, SOL_SOCKET, SO_SNDBUF, &len, sizeof(len));
}

int ice::lib_tcp_t::set_reuse_addr( int s )
{
	const int flag = 1;
	return setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
}

int ice::lib_tcp_cli_t::safe_tcp_connect( const char* ipaddr, in_port_t port, int timeout, bool block )
{
	struct sockaddr_in peer;

	memset(&peer, 0, sizeof(peer));
	peer.sin_family  = AF_INET;
	peer.sin_port    = htons(port);
	if (inet_pton(AF_INET, ipaddr, &peer.sin_addr) <= 0) {
		return -1;
	}

	int sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if ( -1 == sockfd) {
		return -1;
	}

	//------------------------
	// Works under Linux, although **UNDOCUMENTED**!!
	// 设置超时无用.要用select判断. 见unix网络编程connect
	if (timeout > 0) {
		ice::lib_tcp_t::set_sock_send_timeo(sockfd, timeout * 1000);
	}
	if (-1 == connect(sockfd, (struct sockaddr*)&peer, sizeof(peer))) {
		close(sockfd);
		return -1;
	}
	if (timeout > 0) {
		ice::lib_tcp_t::set_sock_send_timeo(sockfd, 0);
	}

	ice::lib_file_t::set_io_block(sockfd, block);

	return sockfd;
}

int ice::lib_tcp_sever_t::safe_tcp_accept( int sockfd, struct sockaddr_in* peer, bool block )
{
	int err;
	int newfd;
	socklen_t peer_size = sizeof(*peer);
	while(1) {
		newfd = accept(sockfd, (struct sockaddr*)peer, &peer_size);
		if (newfd >= 0) {
			break;
		} else if (errno != EINTR) {
			return -1;
		}
	}

	if (!block && (lib_file_t::set_io_block(newfd, false) == -1)) {
		err   = errno;
		close(newfd);
		errno = err;
		return -1;
	}

	return newfd;
}

int ice::lib_tcp_sever_t::safe_socket_listen( const char* ipaddr, in_port_t port, int backlog, int bufsize )
{
	assert((backlog > 0) && (bufsize > 0) && (bufsize <= (10 * 1024 * 1024)));

	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family  = PF_INET;
	servaddr.sin_port    = htons(port);
	if (NULL != ipaddr) {
		inet_pton(PF_INET, ipaddr, &servaddr.sin_addr);
	} else {
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	}

	int listenfd;
	if (-1 == (listenfd = socket(PF_INET, SOCK_STREAM, 0))) {
		return -1;
	}

	int err = lib_tcp_t::set_reuse_addr(listenfd);
	if (-1 == err) {
		goto ret;
	}

	err = lib_tcp_t::set_recvbuf(listenfd, bufsize);
	if (-1 == err) {
		goto ret;
	}

	err = lib_tcp_t::set_sendbuf(listenfd, bufsize);
	if (-1 == err) {
		goto ret;
	}

	err = bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	if (-1 == err) {
		goto ret;
	}

	if (-1 == listen(listenfd, backlog)) {
		err = -1;
		goto ret;
	}

ret:
	if (err) {
		err      = errno;
		close(listenfd);
		listenfd = -1;
		errno    = err;		
	}

	return listenfd;
}

int ice::lib_tcp_sever_t::create_passive_endpoint( const char* host, const char* serv, int backlog, int bufsize )
{
	assert((backlog > 0) && (bufsize > 0) && (bufsize <= (10 * 1024 * 1024)));

	struct addrinfo  hints;
	struct addrinfo* res = 0;

	memset(&hints, 0, sizeof(hints));
	hints.ai_flags    = AI_PASSIVE;
	hints.ai_family   = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	int err = getaddrinfo(host, serv, &hints, &res);
	if (err != 0) {
		errno = lib_net_util_t::eai_to_errno(err);
		return -1;
	}

	int listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (listenfd < 0) {
		freeaddrinfo(res);
		return -1;
	}

	err = lib_tcp_t::set_reuse_addr(listenfd);
	if (err == -1) {
		goto ret;
	}

	err = setsockopt(listenfd, SOL_SOCKET, SO_RCVBUF, &bufsize, sizeof(int));
	if (err == -1) {
		goto ret;
	}
	err = setsockopt(listenfd, SOL_SOCKET, SO_SNDBUF, &bufsize, sizeof(int));
	if (err == -1) {
		goto ret;
	}

	err = bind(listenfd, res->ai_addr, res->ai_addrlen);
	if (err < 0) {
		goto ret;
	}

	if (-1 == listen(listenfd, backlog)) {
		err = -1;
		goto ret;
	}

ret:
	if (err) {
		err      = errno;
		close(listenfd);
		listenfd = -1;
	}
	freeaddrinfo(res);
	errno = err;

	return listenfd;
}

int ice::lib_tcp_server_epoll_t::run()
{
	if (0 == this->max_events_num){
		return -1;
	}
	while (this->is_run){
	}
	
	return 0;
}


void ice::lib_tcp_server_epoll_t::init( uint32_t maxevents )
{
	this->max_events_num = maxevents;
}

ice::lib_tcp_server_epoll_t::lib_tcp_server_epoll_t()
{
	this->max_events_num = 0;
	this->is_run = true;
}
