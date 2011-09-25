/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	读取bench.ini配置文件
	brief:		
*********************************************************************/
#pragma once

#include <stdint.h>
#include <netinet/in.h>
#include <string>
#include <vector>

#include "shmq.h"

struct bind_config_elem_t {
	uint32_t		online_id;
	std::string		online_name;
	std::string		bind_ip;
	in_port_t		bind_port;
	uint8_t			restart_cnt;//重启过的次数
	shm_queue_t		sendq;
	shm_queue_t		recvq;
};

class bind_config_t
{
public:
	std::vector<bind_config_elem_t> elems;

	//************************************
	// Brief:     加载配置文件
	// Returns:   int
	//************************************
	int load();
	//************************************
	// Brief:     获取服务器元素个数
	// Returns:   uint32_t
	//************************************
	uint32_t get_elem_num() const;
	int get_bind_conf_idx(const bind_config_elem_t* bc_elem) const;
protected:
private:
};

extern bind_config_t g_bind_conf;