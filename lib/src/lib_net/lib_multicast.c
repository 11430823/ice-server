#include "lib_include.h"
#include "lib_log.h"
#include "lib_net/lib_tcp.h"
#include "lib_net/lib_multicast.h"

int ice::lib_multicast_t::join_multicast( int s )
{
	ip_mreq_source mreq;
	mreq.imr_interface.s_addr   =   inet_addr( "127.0.0.1 "); //自身地址
	mreq.imr_multiaddr.s_addr   =   inet_addr( "239.8.7.6 "); //加入组播的地址
	::setsockopt(this->fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
	return 0;
}

int ice::lib_multicast_t::exit_multicast( int s )
{
	ip_mreq_source mreq;
	::setsockopt(s, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
	return 0;
}

int ice::lib_multicast_t::refuse_multicast( int s )
{
	ip_mreq_source mreq;
	mreq.imr_interface.s_addr   =   inet_addr( "127.0.0.1 "); //自身地址
	mreq.imr_multiaddr.s_addr   =   inet_addr( "239.8.7.6 "); //加入组播的地址
	mreq.imr_sourceaddr.s_addr = inet_addr("192.168.0.111");//阻止的地址
	::setsockopt(s, IPPROTO_IP, IP_BLOCK_SOURCE, (char*)&mreq, sizeof(mreq));
	return 0;
}

static struct sockaddr_in mcast_addr;
int ice::lib_multicast_t::create(const char* ip, uint16_t port, const char* outgoing_ip, const char* incoming_ip)
{
	this->fd = ::socket(AF_INET, SOCK_DGRAM, 0);
	if (-1 == this->fd) {
		ERROR_RETURN(-1, ("failed to create mcast_fd [err:%d, %s]", errno, strerror(errno)));
	}

	memset(&mcast_addr, 0, sizeof(mcast_addr));
	mcast_addr.sin_family = AF_INET;
	::inet_pton(AF_INET, ip, &(mcast_addr.sin_addr));
	mcast_addr.sin_port = htons(port);

	lib_tcp_t::set_reuse_addr(this->fd);

	int loop = 1;
	if (-1 == ::setsockopt(this->fd, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop))){
		ERROR_RETURN(-1, ("failed to set ip_multicast_loop: [err:%d, %s]", errno, strerror(errno)));
	}

	// Set Default Interface For Outgoing Multicasts
	in_addr_t ipaddr;
	::inet_pton(AF_INET, outgoing_ip, &ipaddr);
	if (::setsockopt(this->fd, IPPROTO_IP, IP_MULTICAST_IF, &ipaddr, sizeof ipaddr) == -1) {
		ERROR_RETURN(-1, ("failed to set outgoing interface: err=%d %s %s",
			errno, strerror(errno), outgoing_ip));
	}

	if (::bind(this->fd, (struct sockaddr*)&mcast_addr, sizeof mcast_addr) == -1) {
		ERROR_RETURN(-1, ("Failed to Bind `mcast_fd`: err=%d %s", errno, strerror(errno)));
	}

	// Join the Multicast Group
	this->join_multicast(this->fd);
	/*
	struct group_req req;
	req.gr_interface = if_nametoindex(config_get_strval("mcast_incoming_if"));
	memcpy(&req.gr_group, &mcast_addr, sizeof mcast_addr);
	if (::setsockopt(mcast_fd, IPPROTO_IP, MCAST_JOIN_GROUP, &req, sizeof req) == -1) {
		ERROR_RETURN(("Failed to Join Mcast Grp: err=%d %s", errno, strerror(errno)), -1);
	}
	*/

	//return do_add_conn(mcast_fd, fd_type_mcast, &mcast_addr, 0);
	return 0;
}
