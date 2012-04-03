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

enum {
	E_PIPE_INDEX_RDONLY = 0,
	E_PIPE_INDEX_WRONLY = 1,
	E_PIPE_INDEX_MAX,
};

#pragma pack(1)

//父子进程之间的管道.
struct pipe_t {
	int pipe_handles[E_PIPE_INDEX_MAX];
	pipe_t();
	int create();
};

//每个进程的数据
struct bind_config_elem_t {
	uint32_t		id;//序号
	std::string		name;//名称
	uint8_t			net_type;//网络类型
	std::string		ip;
	in_port_t		port;
	uint8_t			restart_cnt;//重启过的次数(防止不断的重启)
	pipe_t			send_pipe;//针对子进程的写
	pipe_t			recv_pipe;//针对子进程的读
	bind_config_elem_t();
	uint32_t get_id();
	const char* get_name();
	const char* get_ip();
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
		return this->elems.size();
	}
	bind_config_elem_t* get_elem(uint32_t index);
	int get_elem_idx(const bind_config_elem_t* bc_elem);
	void add_elem(const bind_config_elem_t& elem);
protected:
private:
	std::vector<bind_config_elem_t> elems;
};

extern bind_config_t g_bind_conf;