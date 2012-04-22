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
	int s2peer(ice::lib_tcp_peer_info_t* cli_info, const void* data, uint32_t len);
	int connect(ice::lib_tcp_peer_info_t* svr_info);
}//end namespace fire
