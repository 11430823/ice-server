#include "lib_net/lib_udp.h"

int ice::lib_udp_t::send( const void* buf, int total )
{
	return HANDLE_EINTR(::sendto(this->fd, buf, total, MSG_NOSIGNAL, (sockaddr*)&(this->addr), sizeof(this->addr)));
}

int ice::lib_udp_t::recv( void* buf, int bufsize )
{
	//todo return HANDLE_EINTR(::recvfrom(this->fd, buf, bufsize, 0));
	return ERR;
}

int ice::lib_udp_t::connect( const std::string& ip, uint16_t port )
{
	bzero(this->addr, sizeof(this->addr));
	this->addr->sin_family = AF_INET;
	this->addr->sin_port = htons(port);
	inet_pton(AF_INET, ip.c_str(), &(this->addr->sin_addr));
	this->fd = socket(AF_INET, SOCK_DGRAM, 0);
	return this->fd;
}
