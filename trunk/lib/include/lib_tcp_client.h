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
	class lib_tcp_client_t : public lib_tcp_t
	{
	public:
		uint8_t		fd_type;
		uint16_t	remote_port;
		uint32_t	remote_ip;
		uint32_t	last_tm;
		lib_active_buf_t recv_buf;
		lib_active_buf_t send_buf;
	public:
		lib_tcp_client_t();
		virtual ~lib_tcp_client_t();
		char* get_ip();
		void close();
		void init();
		static int send(const void* buf, int total);
	protected:
		
	private:
		lib_tcp_client_t(const lib_tcp_client_t& cr);
		lib_tcp_client_t& operator=(const lib_tcp_client_t& cr);
	};
}//end namespace ice