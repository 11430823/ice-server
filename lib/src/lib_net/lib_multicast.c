#include "lib_include.h"
#include "lib_log.h"
#include "lib_net/lib_tcp.h"
#include "lib_file.h"

#include "lib_net/lib_multicast.h"

int ice::lib_mcast_t::create(const std::string& mcast_ip, uint16_t mcast_port,
								 const std::string& mcast_incoming_if, const std::string& mcast_outgoing_if)
{
	this->mcast_ip = mcast_ip;
	this->mcast_port = mcast_port;
	this->mcast_incoming_if = mcast_incoming_if;
	this->mcast_outgoing_if = mcast_outgoing_if;

	this->fd = ::socket(AF_INET, SOCK_DGRAM, 0);
	if (-1 == this->fd) {
		ERROR_RETURN(-1, ("failed to create mcast_fd [err_code:%d, err:%s]", errno, strerror(errno)));
	}

	lib_file_t::set_io_block(this->fd, false);

	memset(&this->addr, 0, sizeof(this->addr));
	this->addr.sin_family = AF_INET;
	::inet_pton(AF_INET, this->mcast_ip.c_str(), &(this->addr.sin_addr));
	this->addr.sin_port = htons(this->mcast_port);

	lib_tcp_t::set_reuse_addr(this->fd);
#if 0//�鲥�Լ����յ�
	int loop = 1;
	if (-1 == ::setsockopt(this->fd, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop))){
		ERROR_RETURN(-1, ("failed to set ip_multicast_loop [err_code:%d, err:%s]", errno, strerror(errno)));
	}
#endif
	// set default interface for outgoing multicasts
	in_addr_t ipaddr;
	::inet_pton(AF_INET, this->mcast_outgoing_if.c_str(), &ipaddr);
	if (::setsockopt(this->fd, IPPROTO_IP, IP_MULTICAST_IF, &ipaddr, sizeof ipaddr) == -1) {
		ERROR_RETURN(-1, ("failed to set outgoing interface [err_code:%d, err:%s, ip:%s]",
			errno, strerror(errno), this->mcast_outgoing_if.c_str()));
	}

	if (::bind(this->fd, (struct sockaddr*)&this->addr, sizeof(this->addr)) == -1) {
		ERROR_RETURN(-1, ("failed to bind mcast_fd [err_code:%d, err:%s]", errno, strerror(errno)));
	}

	// Join the Multicast Group
	struct group_req req;
	req.gr_interface = if_nametoindex(this->mcast_incoming_if.c_str());
	memcpy(&req.gr_group, &this->addr, sizeof(this->addr));
	if (-1 == ::setsockopt(this->fd, IPPROTO_IP, MCAST_JOIN_GROUP, &req, sizeof(req))) {
		ERROR_RETURN(-1, ("failed to join mcast grp [err_code:%d, err:%s]", errno, strerror(errno)));
	}

	return 0;
}
