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

		/**
		* @brief Create a TCP connection
		*
		* @param const char* ip,  the ip address to connect to.
		* @param uint16_t port,  the port to connect to.
		* @param int timeout,  abort the connecting attempt after timeout secs. If timeout is less than or equal to 0, 
		*                                then the connecting attempt will not be interrupted until error occurs.
		* @param int block,  true and the connected fd will be set blocking, false and the fd will be set nonblocking.
		*
		* @return int, the connected fd on success, -1 on error.
		*/
		// todo need test connect timeout [3/11/2012 meng]
		static int connect( const char* ip, uint16_t port, int timeout, bool block );
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
