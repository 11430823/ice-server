#include "lib_include.h"
#include "lib_file.h"
#include "lib_net.h"

ice::lib_net_t::lib_net_t()
{
	this->fd = -1;
}

ice::lib_net_t::~lib_net_t()
{
	lib_file_t::close_fd(this->fd);
}

int ice::lib_net_t::set_sock_send_timeo( int sockfd, int millisec )
{
	struct timeval tv;

	tv.tv_sec  = millisec / 1000;
	tv.tv_usec = (millisec % 1000) * 1000;

	return setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
}

int ice::lib_net_t::set_sock_rcv_timeo( int sockfd, int millisec )
{
	struct timeval tv;

	tv.tv_sec  = millisec / 1000;
	tv.tv_usec = (millisec % 1000) * 1000;

	return setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
}

int ice::lib_net_t::set_recvbuf( int s, uint32_t len )
{
	return  setsockopt(s, SOL_SOCKET, SO_RCVBUF, &len, sizeof(len));
}

int ice::lib_net_t::set_sendbuf( int s, uint32_t len )
{
	return setsockopt(s, SOL_SOCKET, SO_SNDBUF, &len, sizeof(len));
}