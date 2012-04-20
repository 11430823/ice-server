#include "lib_include.h"
#include "lib_multicast.h"

int ice::lib_multicast_t::join_multicast( int s )
{
	ip_mreq_source mreq;
	mreq.imr_interface.s_addr   =   inet_addr( "127.0.0.1 "); //�����ַ
	mreq.imr_multiaddr.s_addr   =   inet_addr( "234.5.6.7 "); //�����鲥�ĵ�ַ
	::setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
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
	mreq.imr_interface.s_addr   =   inet_addr( "127.0.0.1 "); //�����ַ
	mreq.imr_multiaddr.s_addr   =   inet_addr( "234.5.6.7 "); //�����鲥�ĵ�ַ
	mreq.imr_sourceaddr.s_addr = inet_addr("192.168.0.111");//��ֹ�ĵ�ַ
	::setsockopt(s, IPPROTO_IP, IP_BLOCK_SOURCE, (char*)&mreq, sizeof(mreq));
	return 0;
}

ice::lib_multicast_t::lib_multicast_t()
{

}