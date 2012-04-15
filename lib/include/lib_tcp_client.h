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
	public:
		lib_tcp_client_t(){
			this->init();
		}
		virtual ~lib_tcp_client_t(){}
		char* get_str_ip(){
			struct in_addr a;
			a.s_addr = this->remote_ip;
			return inet_ntoa(a);
		}
		void close(){
			lib_file_t::close_fd(this->fd);
			this->init();
		}
		void init(){
			this->fd = -1;
			this->fd_type = FD_TYPE_UNUSED;
			this->remote_port = 0;
			this->remote_ip = 0;
			this->last_tm = 0;
			this->recv_buf.clean();
		}
	protected:
		
	private:
		lib_tcp_client_t(const lib_tcp_client_t& cr);
		lib_tcp_client_t& operator=(const lib_tcp_client_t& cr);
	};
}//end namespace ice