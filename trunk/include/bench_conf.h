/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	读取bench.ini配置文件
	brief:		
*********************************************************************/
#pragma once

#include <string>

class bench_conf_t 
{
public:
	bench_conf_t();
	//************************************
	// Brief:     加载配置文件bench.ini
	// Returns:   int(0:正确,其它:错误)
	//************************************
	int load();
	const char* get_liblogic_path() const;
	uint32_t get_max_fd_num()const;
	bool is_daemon();
	//获取配置项数据
	std::string get_strval(std::string& key, std::string& name);
	std::string log_dir;//日志目录
	uint32_t log_level;//日志等级
	uint32_t log_max_size;//日志每个文件的最大大小(字节)
	uint32_t log_max_files;//日志文件的最大数量
	uint32_t log_save_next_file_interval_min;//每多少时间(分钟)重新保存文件中(新文件)
private:
	std::string libdata_path;//数据段SO路径
	std::string liblogic_path;//代码段SO路径
	uint32_t max_fd_num;//打开文件的最大数量
	int daemon;//是否后台运行 1:是, 0:否
};

extern bench_conf_t g_bench_conf;
