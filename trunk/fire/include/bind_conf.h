/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	读取bind.xml配置文件
	brief:		ok
*********************************************************************/
#pragma once

#include <stdint.h>
#include <string>
#include <vector>

#include "bind_pipe.h"

#pragma pack(1)

//每个进程的数据
struct bind_config_elem_t {
	uint32_t		id;//序号
	std::string		name;//名称
	uint8_t			net_type;//网络类型
	std::string		ip;
	uint16_t		port;
	uint8_t			restart_cnt;//重启过的次数(防止不断的重启)
	bind_pipe_t			send_pipe;//针对子进程的写
	bind_pipe_t			recv_pipe;//针对子进程的读
	bind_config_elem_t();
};
#pragma pack()

class bind_config_t
{
public:
	std::vector<bind_config_elem_t> elems;

public:
	//************************************
	// Brief:     加载配置文件
	// Returns:   int 0:success, -1:error
	//************************************
	int load();
	int get_elem_idx(const bind_config_elem_t* bc_elem);
protected:
private:
};

extern bind_config_t g_bind_conf;