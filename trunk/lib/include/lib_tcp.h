/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		ok
*********************************************************************/

#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "lib_util.h"
#include "lib_net.h"

namespace ice{
	class lib_tcp_t : public lib_net_t
	{
		PROTECTED_RW_DEFAULT(uint16_t, port);
		PROTECTED_RW_DEFAULT(uint32_t, ip);
	public:
		lib_tcp_t(){
			this->port = 0;
			this->ip = 0;
		}
		virtual ~lib_tcp_t(){}

		virtual int send(const void* buf, int total);

		virtual int recv(void* buf, int bufsize);

		const char* get_ip_str();

		//************************************
		// Brief:	  Set the given fd SO_REUSEADDR
		// Returns:   int (0 on success, -1 on error)
		// Parameter: int s (fd)
		//************************************
		static int set_reuse_addr(int s);

	protected:
		
	private:
		lib_tcp_t(const lib_tcp_t& cr);
		lib_tcp_t& operator=(const lib_tcp_t& cr);
	};

}//end namespace ice
