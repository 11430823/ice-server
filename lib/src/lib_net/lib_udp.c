#include "lib_net/lib_udp.h"

int ice::lib_udp_t::send( const void* buf, int total )
{
	return HANDLE_EINTR(::sendto(this->fd, buf, total, MSG_NOSIGNAL, (sockaddr*)&(this->addr), sizeof(this->addr)));
}

int ice::lib_udp_t::recv( void* buf, int bufsize )
{
	//todo return HANDLE_EINTR(::recvfrom(this->fd, buf, bufsize, 0));
	return 0;
}
