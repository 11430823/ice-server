/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		tcp¿Í»§¶Ë
*********************************************************************/

#pragma once

#include "lib_util.h"
#include "lib_net_util.h"
#include "lib_tcp.h"
#include "lib_memory.h"
#include "lib_file.h"

namespace ice{
	class lib_tcp_peer_info_t : public lib_tcp_t
	{
		PROTECTED_RW_DEFAULT(uint32_t, last_tm);
	public:
		uint8_t		fd_type;
		lib_active_buf_t recv_buf;
		lib_active_buf_t send_buf;
	public:
		lib_tcp_peer_info_t();
		virtual ~lib_tcp_peer_info_t();
		virtual uint32_t get_ip();
		virtual uint16_t get_port();
	public:
		void close();
		void init();
				/**
		* @brief Create a TCP connection
		*
		* @param const char* ipaddr,  the ip address to connect to.
		* @param in_port_t port,  the port to connect to.
		* @param int timeout,  abort the connecting attempt after timeout secs. If timeout is less than or equal to 0, 
		*                                then the connecting attempt will not be interrupted until error occurs.
		* @param int block,  true and the connected fd will be set blocking, false and the fd will be set nonblocking.
		*
		* @return int, the connected fd on success, -1 on error.
		*/
		// todo need test connect timeout [3/11/2012 meng]
		int connect( int timeout, bool block );
	protected:
	private:
		lib_tcp_peer_info_t(const lib_tcp_peer_info_t& cr);
		lib_tcp_peer_info_t& operator=(const lib_tcp_peer_info_t& cr);
	};
}//end namespace ice