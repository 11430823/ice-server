#include "lib_include.h"
#include "lib_file.h"
#include "lib_net/lib_net_util.h"
#include "lib_log.h"
#include "lib_net/lib_tcp.h"

int ice::lib_tcp_t::set_reuse_addr( int s )
{
	const int flag = 1;
	return ::setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
}

int ice::lib_tcp_t::send( const void* data, int len )
{
	int send_bytes = 0;
	for (int cur_len = 0; send_bytes < len; send_bytes += cur_len) {
		//MSG_NOSIGNAL: linux man send 查看
		cur_len = ::send(this->fd, (char*)data + send_bytes, len - send_bytes, MSG_NOSIGNAL);
		if (-1 == cur_len) {
			if (EINTR == errno) {
				cur_len = 0;
			} else if (EAGAIN == errno || EWOULDBLOCK == errno) {
				ALERT_LOG("send err [err_code:%u, err_msg:%s]", errno, ::strerror(errno));
				break;
			} else {
				ALERT_LOG("send err [err_code:%u, err_msg:%s]", errno, ::strerror(errno));
				return ERR;
			}
		}
	}
	return send_bytes;
}

int ice::lib_tcp_t::recv( void* data, int len )
{
	return HANDLE_EINTR(::recv(this->fd, data, len, 0));
}

std::string ice::lib_tcp_t::get_ip_str()
{
	return ice::lib_net_util_t::ip2str(this->ip);
}

int ice::lib_tcp_t::connect( const std::string& ip, uint16_t port, int timeout, bool block, uint32_t send_buf_len, uint32_t rev_buf_len )
{
	struct sockaddr_in peer;
	::memset(&peer, 0, sizeof(peer));
	peer.sin_family  = AF_INET;
	peer.sin_port    = ::htons(port);
	if (::inet_pton(AF_INET, ip.c_str(), &peer.sin_addr) <= 0) {
		return ERR;
	}

	int fd = ::socket(PF_INET, SOCK_STREAM, 0);
	if ( INVALID_FD == fd) {
		ALERT_LOG("create socket err [%s]", ::strerror(errno));
		return ERR;
	}

	//设置发送与接收缓存大小
	if (0 != send_buf_len){
		lib_net_util_t::set_sendbuf(fd, send_buf_len);
	}
	if (0 != rev_buf_len){
		lib_net_util_t::set_recvbuf(fd, rev_buf_len);
	}

	//------------------------
	// Works under Linux, although **UNDOCUMENTED**!!
	// 设置超时无用.要用select判断. 见unix网络编程connect
	if (timeout > 0) {
		ice::lib_net_util_t::set_sock_send_timeo(fd, timeout * 1000);
	}
	if (-1 == HANDLE_EINTR(::connect(fd, (struct sockaddr*)&peer, sizeof(peer)))) {
		ALERT_LOG("connect err [errno:%s, ip:%s, port:%u]", 
			::strerror(errno), ip.c_str(), port);
		lib_file_t::close_fd(fd);
		return ERR;
	}
	if (timeout > 0) {
		ice::lib_net_util_t::set_sock_send_timeo(fd, 0);
	}

	ice::lib_file_t::set_io_block(fd, block);

	return fd;
}

