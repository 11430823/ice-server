#include "lib_include.h"
#include "lib_file.h"
#include "lib_net_util.h"
#include "lib_tcp.h"

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

#if 0
		/**
		* @brief Create a TCP connection
		*
		* @param const char* ipaddr,  the ip address to connect to.
		* @param in_port_t port,  the port to connect to.
		* @param int timeout,  abort the connecting attempt after timeout secs. If timeout is less than or equal to 0, 
		*                                then the connecting attempt will not be interrupted until error occurs.
		* @param int block,  true and the connected fd will be set blocking, false and the fd will be set nonblocking.
		*
		* @return int, the connected fd on success, -1 on error.
		*/
		// todo need test connect timeout [3/11/2012 meng]
int ice::lib_tcp_cli_t::connect( const char* ipaddr, in_port_t port, int timeout, bool block )
{
	struct sockaddr_in peer;

	::memset(&peer, 0, sizeof(peer));
	peer.sin_family  = AF_INET;
	peer.sin_port    = ::htons(port);
	if (::inet_pton(AF_INET, ipaddr, &peer.sin_addr) <= 0) {
		return -1;
	}

	int sockfd = ::socket(PF_INET, SOCK_STREAM, 0);
	if ( -1 == sockfd) {
		return -1;
	}

	//------------------------
	// Works under Linux, although **UNDOCUMENTED**!!
	// 设置超时无用.要用select判断. 见unix网络编程connect
	if (timeout > 0) {
		ice::lib_net_t::set_sock_send_timeo(sockfd, timeout * 1000);
	}
	if (-1 == ::connect(sockfd, (struct sockaddr*)&peer, sizeof(peer))) {
		lib_file_t::close_fd(sockfd);
		return -1;
	}
	if (timeout > 0) {
		ice::lib_tcp_t::set_sock_send_timeo(sockfd, 0);
	}

	ice::lib_file_t::set_io_block(sockfd, block);

	return sockfd;
}
#endif