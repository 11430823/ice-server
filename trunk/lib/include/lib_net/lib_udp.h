/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	ok
	brief:		UDP
*********************************************************************/

#pragma once

#include "lib_include.h"
#include "lib_net.h"

namespace ice{
	class lib_udp_t : public lib_net_t
	{
		PROTECTED_RW(sockaddr_in, addr);
	public:
		lib_udp_t(){}
		virtual ~lib_udp_t(){}
		virtual int send(const void* buf, int total);
		virtual int recv(void* buf, int bufsize);
	protected:
		
	private:
		lib_udp_t(const lib_udp_t& cr);
		lib_udp_t& operator=(const lib_udp_t& cr);
	};
}//end namespace ice