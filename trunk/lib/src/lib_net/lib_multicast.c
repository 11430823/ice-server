#include "lib_include.h"
#include "lib_log.h"
#include "lib_net/lib_tcp.h"
#include "lib_file.h"
#include "lib_net/lib_net_util.h"

#include "lib_net/lib_multicast.h"

int ice::lib_mcast_t::create(const std::string& mcast_ip, uint16_t mcast_port,
								 const std::string& mcast_incoming_if, const std::string& mcast_outgoing_if)
{
	this->mcast_ip = mcast_ip;
	this->mcast_port = mcast_port;
	this->mcast_incoming_if = mcast_incoming_if;
	this->mcast_outgoing_if = mcast_outgoing_if;

	this->fd = ::socket(AF_INET, SOCK_DGRAM, 0);
	if (INVALID_FD == this->fd) {
		ERROR_RETURN(ERR, ("failed to create mcast_fd [err_code:%d, err:%s]", errno, strerror(errno)));
	}

	lib_file_t::set_io_block(this->fd, false);

	memset(&this->addr, 0, sizeof(this->addr));
	/*
	 *ai_family参数指定调用者期待返回的套接口地址结构的类型。
	 *它的值包括三种：AF_INET，AF_INET6和AF_UNSPEC。如果指定AF_INET，
	 *那么函数就不能返回任何IPV6相关的地址信息；如果仅指定了AF_INET6，
	 *则就不能返回任何IPV4地址信息。AF_UNSPEC则意味着函数返回的是适用于指定
	 *主机名和服务名且适合任何协议族的地址。如果某个主机既有AAAA记录(IPV6)地址，
	 *同时又有A记录(IPV4)地址，那么AAAA记录将作为sockaddr_in6结构返回，
	 而A记录则作为sockaddr_in结构返回
	*/
	this->addr.sin_family = AF_INET;//AF_UNSPEC ;
	::inet_pton(AF_INET, this->mcast_ip.c_str(), &(this->addr.sin_addr));
	this->addr.sin_port = htons(this->mcast_port);

	lib_tcp_t::set_reuse_addr(this->fd);

	int loop = 1;
	if (-1 == ::setsockopt(this->fd, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop))){
		ERROR_RETURN(ERR, ("failed to set ip_multicast_loop [err_code:%d, err:%s]", errno, strerror(errno)));
	}

	// set default interface for outgoing multicasts
	in_addr_t ipaddr;
	::inet_pton(AF_INET, this->mcast_outgoing_if.c_str(), &ipaddr);
	if (::setsockopt(this->fd, IPPROTO_IP, IP_MULTICAST_IF, &ipaddr, sizeof ipaddr) == -1) {
		ERROR_RETURN(ERR, ("failed to set outgoing interface [err_code:%d, err:%s, ip:%s]",
			errno, strerror(errno), this->mcast_outgoing_if.c_str()));
	}

	if (::bind(this->fd, (struct sockaddr*)&this->addr, sizeof(this->addr)) == -1) {
		ERROR_RETURN(ERR, ("failed to bind mcast_fd [err_code:%d, err:%s]", errno, strerror(errno)));
	}

	return this->mcast_join();
}

int ice::lib_mcast_t::mcast_join()
{
	struct group_req req;

	req.gr_interface = if_nametoindex(this->mcast_incoming_if.c_str());
	if (req.gr_interface == 0) {
		errno = ENXIO; /* i/f name not found */
		return ERR;
	}

	memcpy(&req.gr_group, &this->addr, sizeof(this->addr));
	if (-1 == ::setsockopt(this->fd, lib_net_util_t::family_to_level(this->addr.sin_family), MCAST_JOIN_GROUP, &req, sizeof(req))) {
		ERROR_RETURN(-1, ("failed to join mcast grp [err_code:%d, err:%s]", errno, strerror(errno)));
	}

	return SUCC;
}




