
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <assert.h>
#include <netdb.h>

#include <lib_file.h>
#include <lib_net_util.h>

#include "lib_tcp.h"

ice::lib_tcp_t::lib_tcp_t()
{
	this->fd = -1;
}

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
	assert(total > 0);

	int send_bytes, cur_len;

	for (send_bytes = 0; send_bytes < total; send_bytes += cur_len) {
		cur_len = send(sockfd, (char*)buf + send_bytes, total - send_bytes, 0);
		if (-1 == cur_len) {
			if (errno == EINTR) {
				cur_len = 0;
			} else if (errno == EAGAIN) {
				return send_bytes;
			} else {
				return -1;
			}
		}
	}

	return send_bytes;
}

int ice::lib_tcp_t::safe_tcp_recv( int sockfd, void* buf, int bufsize )
{
	int cur_len;

recv_again:
	cur_len = recv(sockfd, buf, bufsize, 0);
	if (0 == cur_len) {
		//connection closed by client
		return 0;
	} else if (-1 == cur_len) {
		if (errno == EINTR) {
			goto recv_again;
		}
	}

	return cur_len;
}

int ice::lib_tcp_t::safe_tcp_recv_n( int sockfd, void* buf, int total )
{
	assert(total > 0);

	int recv_bytes, cur_len;

	for (recv_bytes = 0; recv_bytes < total; recv_bytes += cur_len)	{
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

	while(1) {
		socklen_t peer_size = sizeof(*peer);
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

int ice::lib_tcp_sever_t::safe_socket_listen( const char* ipaddr, in_port_t port, int type, int backlog, int bufsize )
{
	assert((backlog > 0) && (bufsize > 0) && (bufsize <= (10 * 1024 * 1024)));

	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family  = AF_INET;
	servaddr.sin_port    = htons(port);
	if (NULL != ipaddr) {
		inet_pton(AF_INET, ipaddr, &servaddr.sin_addr);
	} else {	
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	}

	int listenfd;
	if (-1 == (listenfd = socket(AF_INET, type, 0))) {
		return -1;
	}

	int err;
	if (type != SOCK_DGRAM) {
		int reuse_addr = 1;	
		err = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));
		if (err == -1) {
			goto ret;
		}
	}

	err = setsockopt(listenfd, SOL_SOCKET, SO_RCVBUF, &bufsize, sizeof(int));
	if (err == -1) {
		goto ret;
	}
	err = setsockopt(listenfd, SOL_SOCKET, SO_SNDBUF, &bufsize, sizeof(int));
	if (err == -1) {
		goto ret;
	}

	err = bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	if (err == -1) {
		goto ret;
	}

	if ((type == SOCK_STREAM) && (listen(listenfd, backlog) == -1)) {
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

int ice::lib_tcp_sever_t::create_passive_endpoint( const char* host, const char* serv, int socktype, int backlog, int bufsize )
{
	assert((backlog > 0) && (bufsize > 0) && (bufsize <= (10 * 1024 * 1024)));

	struct addrinfo  hints;
	struct addrinfo* res = 0;

	memset(&hints, 0, sizeof(hints));
	hints.ai_flags    = AI_PASSIVE;
	hints.ai_family   = AF_UNSPEC;
	hints.ai_socktype = socktype;

	int err = getaddrinfo(host, serv, &hints, &res);
	if (err != 0) {
		errno = eai_to_errno(err);
		return -1;
	}

	int listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (listenfd < 0) {
		freeaddrinfo(res);
		return -1;
	}

	if (socktype != SOCK_DGRAM) {
		int reuse_addr = 1;	
		err = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));
		if (err == -1) {
			goto ret;
		}
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

	if ((socktype == SOCK_STREAM) && (listen(listenfd, backlog) == -1)) {
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
