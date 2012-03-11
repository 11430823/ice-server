#include <assert.h>
#include <errno.h>
#include <string.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>

#include "inet_utils.h"

int get_ip_addr(const char* nif, int af, void* ipaddr, size_t len)
{
	assert(((af == AF_INET) && (len >= INET_ADDRSTRLEN)) || ((af == AF_INET6) && (len >= INET6_ADDRSTRLEN)));

	// get a list of network interfaces
	struct ifaddrs* ifaddr;
	if (getifaddrs(&ifaddr) < 0) {
		return -1;
	}

	// walk through linked list
	int err = EADDRNOTAVAIL;
	int ret_code = -1;
	struct ifaddrs* ifa; // maintaining head pointer so we can free list later
	for (ifa = ifaddr; ifa != 0; ifa = ifa->ifa_next) {
		if ((ifa->ifa_addr == 0) || (ifa->ifa_addr->sa_family != af)
			|| strcmp(ifa->ifa_name, nif)) {
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

	freeifaddrs(ifaddr);
	errno = err;
	return ret_code;
}

