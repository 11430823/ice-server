#include "lib_include.h"
#include "lib_file.h"
#include "lib_net_util.h"
#include "lib_log.h"
#include "lib_tcp.h"

int ice::lib_tcp_t::set_reuse_addr( int s )
{
	const int flag = 1;
	return ::setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
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

const char* ice::lib_tcp_t::get_ip_str()
{
	return lib_net_t::ip2str(this->ip);
}

int ice::lib_tcp_t::connect( const char* ip, uint16_t port, int timeout, bool block )
{
	struct sockaddr_in peer;
	::memset(&peer, 0, sizeof(peer));
	peer.sin_family  = AF_INET;
	peer.sin_port    = ::htons(port);
	if (::inet_pton(AF_INET, ip, &peer.sin_addr) <= 0) {
		return -1;
	}

	int fd = ::socket(PF_INET, SOCK_STREAM, 0);
	if ( -1 == fd) {
		ALERT_LOG("create socket err [%s]", ::strerror(errno));
		return -1;
	}

	//todo 可设置发送与接收缓存大小

	//------------------------
	// Works under Linux, although **UNDOCUMENTED**!!
	// 设置超时无用.要用select判断. 见unix网络编程connect
	// 			if (timeout > 0) {
	// 				ice::lib_net_t::set_sock_send_timeo(sockfd, timeout * 1000);
	// 			}
	if (-1 == HANDLE_EINTR(::connect(fd, (struct sockaddr*)&peer, sizeof(peer)))) {
		ALERT_LOG("connect err [errno:%s, ip:%s, port:%u]", 
			::strerror(errno), ip, port);
		lib_file_t::close_fd(fd);
		return -1;
	}
	// 			if (timeout > 0) {
	// 				ice::lib_tcp_t::set_sock_send_timeo(sockfd, 0);
	// 			}

	ice::lib_file_t::set_io_block(fd, block);

	return fd;
}
