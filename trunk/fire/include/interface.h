/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		�û��ӿ�(��ʱ����)
*********************************************************************/

#pragma once

#include <stdint.h>

#include <lib_tcp_server.h>

#include <lib_tcp_client.h>

namespace fire{
	int s2peer(ice::lib_tcp_peer_info_t* peer_info, const void* data, uint32_t len);
	ice::lib_tcp_peer_info_t* connect(const char* ip, uint16_t port);
}//end namespace fire
