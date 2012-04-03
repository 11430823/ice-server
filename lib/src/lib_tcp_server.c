#include <assert.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "lib_file.h"
#include "lib_net_util.h"
#include "lib_tcp_server.h"


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

	if (-1 == ::listen(listenfd, backlog)) {
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

	err = ::setsockopt(listenfd, SOL_SOCKET, SO_RCVBUF, &bufsize, sizeof(int));
	if (err == -1) {
		goto ret;
	}
	err = ::setsockopt(listenfd, SOL_SOCKET, SO_SNDBUF, &bufsize, sizeof(int));
	if (err == -1) {
		goto ret;
	}

	err = ::bind(listenfd, res->ai_addr, res->ai_addrlen);
	if (err < 0) {
		goto ret;
	}

	if (-1 == ::listen(listenfd, backlog)) {
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

void ice::lib_tcp_sever_t::set_cli_time_out_sec( uint32_t time_out_sec )
{
	this->cli_time_out_sec = time_out_sec;
}