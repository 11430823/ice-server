#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>

#include "lib_include.h"
#include "lib_util.h"
#include "lib_net/lib_net_util.h"
#include "lib_file.h"

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

int ice::lib_net_util_t::family_to_level( int family )
{
	int level = -1;

	switch (family) {
	case AF_INET:
		level = IPPROTO_IP;
		break;
	case AF_INET6:
		level = IPPROTO_IPV6;
		break;
	default:
		break;
	}

	return level;
}

bool ice::lib_net_util_t::get_local_ip( std::string& ip )
{
	//////////////////////////////////////////////////////////////////////////
	// Don't use 'gethostbyname(NULL)'. The winsock DLL may be replaced by a DLL from a third party
	// which is not fully compatible to the original winsock DLL. ppl reported crash with SCORSOCK.DLL
	// when using 'gethostbyname(NULL)'.
	char szHost[256];
	if (gethostname(szHost, sizeof szHost) == 0){
		hostent* pHostEnt = gethostbyname(szHost);
		if (pHostEnt != NULL && pHostEnt->h_length == 4 && pHostEnt->h_addr_list[0] != NULL){
			char* pTemp = inet_ntoa(*((in_addr*)pHostEnt->h_addr_list[0]));
			ip = pTemp;
			return true;
		}
	}
	return false;
}

bool ice::lib_net_util_t::get_local_ip( long& ip )
{
	char szHost[256];
	if (gethostname(szHost, sizeof szHost) == 0){
		hostent* pHostEnt = gethostbyname(szHost);
		if (pHostEnt != NULL && pHostEnt->h_length == 4 && pHostEnt->h_addr_list[0] != NULL){
			char* pTemp = inet_ntoa(*((in_addr*)pHostEnt->h_addr_list[0]));
			ip = *((long*)pHostEnt->h_addr_list[0]);
			return true;
		}
	}
	return false;
}






