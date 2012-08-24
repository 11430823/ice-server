/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		用户接口
*********************************************************************/

#pragma once

#include <stdint.h>

#include <lib_net/lib_tcp_server.h>
#include <lib_net/lib_tcp_client.h>
#include <lib_proto/lib_msg.h>

namespace fire{
	bool is_parent();
	int s2peer(ice::lib_tcp_peer_info_t* peer_info, const void* data, uint32_t len);
	int s2peer(ice::lib_tcp_peer_info_t* peer_info, const void* head, uint32_t head_len, ice::lib_msg_t* msg);
	ice::lib_tcp_peer_info_t* connect(const std::string& ip, uint16_t port);
	ice::lib_tcp_peer_info_t* connect(const char* svr_name);
	uint32_t get_server_id();
	const char* get_server_name();
}//end namespace fire
