/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		子进程服务
*********************************************************************/

#pragma once

#include <lib_net/lib_multicast.h>

class service_t
{
public:
	struct bind_config_elem_t*	bind_elem;
public:
	service_t(){}
	void run(bind_config_elem_t* bind_elem, int n_inited_bc);
	uint32_t get_bind_elem_id();
	const char* get_bind_elem_name();
protected:
private:
	ice::lib_multicast_t mcast;
};

extern service_t g_service;