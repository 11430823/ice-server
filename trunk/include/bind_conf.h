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
	bind_config_elem_t();
};

class bind_config_t
{
public:
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
	bind_config_elem_t* get_elem(uint32_t index);
	int get_bind_conf_idx(const bind_config_elem_t* bc_elem) const;
	void add_elem(const bind_config_elem_t& elem);
protected:
private:
	std::vector<bind_config_elem_t> elems;
};

extern bind_config_t g_bind_conf;