#include <lib_log.h>

#include "lib_include.h"
#include "lib_tcp_client.h"

ice::lib_tcp_peer_info_t::lib_tcp_peer_info_t()
{
	this->init();
}

void ice::lib_tcp_peer_info_t::close()
{
	lib_file_t::close_fd(this->fd);
	this->init();
}

void ice::lib_tcp_peer_info_t::init()
{
	this->fd = -1;
	this->fd_type = FD_TYPE_UNUSED;
	this->port = 0;
	this->ip = 0;
	this->last_tm = 0;
	this->recv_buf.clean();
	this->send_buf.clean();
}

ice::lib_tcp_peer_info_t::~lib_tcp_peer_info_t()
{

}

uint16_t ice::lib_tcp_peer_info_t::get_port()
{
	return this->port;
}

uint32_t ice::lib_tcp_peer_info_t::get_ip()
{
	return this->ip;
}

int ice::lib_tcp_peer_info_t::connect( int timeout, bool block )
{
	struct sockaddr_in peer;

	::memset(&peer, 0, sizeof(peer));
	peer.sin_family  = AF_INET;
	peer.sin_port    = ::htons(this->port);
	if (::inet_pton(AF_INET, this->get_ip_str(), &peer.sin_addr) <= 0) {
		return -1;
	}

	this->fd = ::socket(PF_INET, SOCK_STREAM, 0);
	if ( -1 == this->fd) {
		ALERT_LOG("create socket err [%s]", ::strerror(errno));
		return -1;
	}

	//------------------------
	// Works under Linux, although **UNDOCUMENTED**!!
	// 设置超时无用.要用select判断. 见unix网络编程connect
	// 			if (timeout > 0) {
	// 				ice::lib_net_t::set_sock_send_timeo(sockfd, timeout * 1000);
	// 			}
	if (-1 == HANDLE_EINTR(::connect(this->fd, (struct sockaddr*)&peer, sizeof(peer)))) {
		ALERT_LOG("connect err [errno:%s, ip:%s, port:%u]", 
			::strerror(errno), this->get_ip_str(), this->port);
		lib_file_t::close_fd(this->fd);
		return -1;
	}
	// 			if (timeout > 0) {
	// 				ice::lib_tcp_t::set_sock_send_timeo(sockfd, 0);
	// 			}

	ice::lib_file_t::set_io_block(this->fd, block);

	return this->fd;
}
