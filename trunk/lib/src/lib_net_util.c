#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <assert.h>
#include <ifaddrs.h>
#include <string.h>

#include "lib_util.h"
#include "lib_net_util.h"
#include "lib_file.h"

int ice::lib_net_multicast_t::join_multicast( int s )
{
	ip_mreq_source mreq;
	mreq.imr_interface.s_addr   =   inet_addr( "127.0.0.1 "); //自身地址
	mreq.imr_multiaddr.s_addr   =   inet_addr( "234.5.6.7 "); //加入组播的地址
	::setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
	return 0;
}

int ice::lib_net_multicast_t::exit_multicast( int s )
{
	ip_mreq_source mreq;
	::setsockopt(s, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
	return 0;
}

int ice::lib_net_multicast_t::refuse_multicast( int s )
{
	ip_mreq_source mreq;
	mreq.imr_interface.s_addr   =   inet_addr( "127.0.0.1 "); //自身地址
	mreq.imr_multiaddr.s_addr   =   inet_addr( "234.5.6.7 "); //加入组播的地址
	mreq.imr_sourceaddr.s_addr = inet_addr("192.168.0.111");//阻止的地址
	::setsockopt(s, IPPROTO_IP, IP_BLOCK_SOURCE, (char*)&mreq, sizeof(mreq));
	return 0;
}

ice::lib_net_multicast_t::lib_net_multicast_t()
{

}


int ice::lib_net_util_t::eai_to_errno( int eai )
{
	switch (eai) {
			case EAI_ADDRFAMILY:
				return EAFNOSUPPORT;
			case EAI_AGAIN:
				return EAGAIN;
			case EAI_MEMORY:
				return ENOMEM;
			case EAI_SERVICE:
			case EAI_SOCKTYPE:
				return ESOCKTNOSUPPORT;
			case EAI_SYSTEM:
				return errno;
	}
	return EADDRNOTAVAIL;
}

int ice::lib_net_util_t::get_ip_addr( const char* nif, int af, void* ipaddr, size_t len )
{
	assert(((af == AF_INET) && (len >= INET_ADDRSTRLEN)) || ((af == AF_INET6) && (len >= INET6_ADDRSTRLEN)));

	// get a list of network interfaces
	struct ifaddrs* ifaddr;
	if (::getifaddrs(&ifaddr) < 0) {
		return -1;
	}

	// walk through linked list
	int err = EADDRNOTAVAIL;
	int ret_code = -1;
	struct ifaddrs* ifa; // maintaining head pointer so we can free list later
	for (ifa = ifaddr; ifa != 0; ifa = ifa->ifa_next) {
		if ((ifa->ifa_addr == 0) || (ifa->ifa_addr->sa_family != af)
			|| ::strcmp(ifa->ifa_name, nif)) {
				continue;
		}
		// convert binary form ip address to numeric string form
		ret_code = getnameinfo(ifa->ifa_addr,
			(af == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6),
			(char*)ipaddr, len, 0, 0, NI_NUMERICHOST);
		if (ret_code != 0) {
			if (ret_code == EAI_SYSTEM) {
				err = errno;
			}
			ret_code = -1;
		}
		break;
	}

	::freeifaddrs(ifaddr);
	errno = err;
	return ret_code;
}


ice::lib_net_t::lib_net_t()
{
	this->fd = -1;
}

ice::lib_net_t::~lib_net_t()
{
	lib_file_t::close_fd(this->fd);
}
