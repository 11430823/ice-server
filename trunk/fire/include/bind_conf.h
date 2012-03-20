/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	读取bench.ini配置文件
	brief:		ok
*********************************************************************/
#pragma once

#include <stdint.h>
#include <netinet/in.h>
#include <string>
#include <vector>

#include "daemon.h"

#pragma pack(1)
struct bind_config_elem_t {
	uint32_t		id;
	std::string		name;
	uint8_t			net_type;
	std::string		ip;
	in_port_t		port;
	uint8_t			restart_cnt;//重启过的次数
	pipe_t			send_pipe;
	pipe_t			recv_pipe;
	bind_config_elem_t();
	uint32_t get_id();
	std::string& get_name();
	std::string& get_ip();
	in_port_t get_port();
};
#pragma pack()

class bind_config_t
{
public:
	//************************************
	// Brief:     加载配置文件
	// Returns:   int 0:success, -1:error
	//************************************
	int load();
	//************************************
	// Brief:     获取逻辑服务器元素个数
	// Returns:   uint32_t
	//************************************
	inline uint32_t get_elem_num() const{
		return m_elems.size();
	}
	bind_config_elem_t* get_elem(uint32_t index);
	int get_elem_idx(const bind_config_elem_t* bc_elem);
	void add_elem(const bind_config_elem_t& elem);
protected:
private:
	std::vector<bind_config_elem_t> m_elems;
};

extern bind_config_t g_bind_conf;