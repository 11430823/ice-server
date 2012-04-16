
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

int ice::lib_tcp_t::send( const void* buf, int total )
{
	int send_bytes = 0;
	for (int cur_len = 0; send_bytes < total; send_bytes += cur_len) {
		//MSG_NOSIGNAL: linux man send 查看
		cur_len = ::send(this->fd, (char*)buf + send_bytes, total - send_bytes, MSG_NOSIGNAL);
		if (-1 == cur_len) {
			if (EINTR == errno) {
				cur_len = 0;
			} else if (EAGAIN == errno || EWOULDBLOCK == errno) {
				break;
			} else {
				return -1;
			}
		}
	}
	return send_bytes;
}

int ice::lib_tcp_t::recv( void* buf, int bufsize )
{
	return HANDLE_EINTR(::recv(this->fd, buf, bufsize, 0));
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



