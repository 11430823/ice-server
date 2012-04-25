#include "lib_include.h"
#include "lib_log.h"
#include "lib_net/lib_tcp.h"
#include "lib_file.h"

#include "lib_net/lib_multicast.h"

int ice::lib_multicast_t::create(const std::string& mcast_ip, uint16_t mcast_port, const std::string& mcast_incoming_if, const std::string& mcast_outgoing_if)
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

	memset(&this->mcast_addr, 0, sizeof(this->mcast_addr));
	this->mcast_addr.sin_family = AF_INET;
	::inet_pton(AF_INET, this->mcast_ip.c_str(), &(this->mcast_addr.sin_addr));
	this->mcast_addr.sin_port = htons(this->mcast_port);

	lib_tcp_t::set_reuse_addr(this->fd);

	int loop = 1;
	if (-1 == ::setsockopt(this->fd, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop))){
		ERROR_RETURN(-1, ("failed to set ip_multicast_loop [err_code:%d, err:%s]", errno, strerror(errno)));
	}

	// set default interface for outgoing multicasts
	in_addr_t ipaddr;
	::inet_pton(AF_INET, this->mcast_outgoing_if.c_str(), &ipaddr);
	if (::setsockopt(this->fd, IPPROTO_IP, IP_MULTICAST_IF, &ipaddr, sizeof ipaddr) == -1) {
		ERROR_RETURN(-1, ("failed to set outgoing interface [err_code:%d, err:%s, ip:%s]",
			errno, strerror(errno), this->mcast_outgoing_if.c_str()));
	}

	if (::bind(this->fd, (struct sockaddr*)&this->mcast_addr, sizeof(this->mcast_addr)) == -1) {
		ERROR_RETURN(-1, ("failed to bind mcast_fd [err_code:%d, err:%s]", errno, strerror(errno)));
	}

	// Join the Multicast Group
	struct group_req req;
	req.gr_interface = if_nametoindex(this->mcast_incoming_if.c_str());
	memcpy(&req.gr_group, &this->mcast_addr, sizeof(this->mcast_addr));
	if (-1 == ::setsockopt(this->fd, IPPROTO_IP, MCAST_JOIN_GROUP, &req, sizeof(req))) {
		ERROR_RETURN(-1, ("failed to join mcast grp [err_code:%d, err:%s]", errno, strerror(errno)));
	}

	return 0;
}

void ice::lib_addr_multicast_t::pack_this_service_info( uint32_t svr_id,
		const char* svr_name, const char* svr_ip, uint16_t svr_port, E_ADDR_MCAST_PKG_TYPE pkg_type)
{
	char* data = new char[sizeof(this->hdr) + sizeof(this->pkg)];
	this->hdr.pkg_type = pkg_type;
	this->hdr.proto_type = MCAST_NOTIFY_ADDR;
	this->pkg.svr_id     = svr_id;
	strcpy(this->pkg.name, svr_name);
	strcpy(this->pkg.ip, svr_ip);
	this->pkg.port = svr_port;
	memcpy(data, &(this->hdr), sizeof(this->hdr));
	memcpy(data + sizeof(this->hdr), &(this->pkg), sizeof(this->pkg));
	this->send(data, sizeof(this->hdr) + sizeof(this->pkg));
	safe_delete_arr(data);
}

ice::lib_addr_multicast_t::addr_mcast_pkg_t::addr_mcast_pkg_t()
{
	this->svr_id = 0;
	memset(this->name, 0, sizeof(this->name));
	memset(this->ip, 0, sizeof(this->ip));
	this->port = 0;
}

ice::mcast_pkg_header_t::mcast_pkg_header_t()
{
	this->pkg_type = 0;
	this->proto_type = 0;
}
