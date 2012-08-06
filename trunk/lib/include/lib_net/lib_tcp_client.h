/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	todo
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
		PROTECTED_RW(uint32_t, last_tm);
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

	protected:
	private:
		lib_tcp_peer_info_t(const lib_tcp_peer_info_t& cr);
		lib_tcp_peer_info_t& operator=(const lib_tcp_peer_info_t& cr);
	};
}//end namespace ice