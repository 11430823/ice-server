#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#include "lib_net_util.h"

int ice::lib_net_multicast::join_multicast( int s )
{
	ip_mreq_source mreq;
	mreq.imr_interface.s_addr   =   inet_addr( "127.0.0.1 "); //自身地址
	mreq.imr_multiaddr.s_addr   =   inet_addr( "234.5.6.7 "); //加入组播的地址
	setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
	return 0;
}

int ice::lib_net_multicast::exit_multicast( int s )
{
	ip_mreq_source mreq;
	setsockopt(s, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
	return 0;
}

int ice::lib_net_multicast::refuse_multicast( int s )
{
	ip_mreq_source mreq;
	mreq.imr_interface.s_addr   =   inet_addr( "127.0.0.1 "); //自身地址
	mreq.imr_multiaddr.s_addr   =   inet_addr( "234.5.6.7 "); //加入组播的地址
	mreq.imr_sourceaddr.s_addr = inet_addr("192.168.0.111");//阻止的地址
	setsockopt(s, IPPROTO_IP, IP_BLOCK_SOURCE, (char*)&mreq, sizeof(mreq));
	return 0;
}

ice::lib_net_multicast::lib_net_multicast()
{

}


int ice::lib_net_util::eai_to_errno( int eai )
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

int ice::lib_net_util::close_socket( int& s )
{
	int nRes = 0;
	if (-1 != s){
		nRes = close(s);
		if(-1 == nRes){
		}
		s = -1;
	}
	return nRes;
}

ice::lib_net::lib_net()
{
	this->fd = -1;
}

ice::lib_net::~lib_net()
{
	lib_net_util::close_socket(this->fd);
}
