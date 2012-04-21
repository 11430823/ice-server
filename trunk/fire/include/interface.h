/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		用户接口(暂时无用)
*********************************************************************/

#pragma once

#include <stdint.h>

#include <lib_tcp_server.h>

#include <lib_tcp_client.h>

namespace fire{

	struct client_info_t : public ice::lib_tcp_client_t
	{
		virtual uint16_t get_port();
		virtual uint32_t get_ip();
		virtual char* get_ip_str();
		virtual int send(const void* data, uint32_t len);
	};

}//end namespace fire
