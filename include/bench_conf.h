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
//so调用获取配置项bench.ini中的数据(自行配置)
	//获取配置项数据
	std::string get_strval(std::string& key, std::string& name);
public:
	bench_conf_t();
	//************************************
	// Brief:     加载配置文件bench.ini
	// Returns:   int(0:正确,其它:错误)
	//************************************
	int load();
	uint32_t get_max_fd_num()const;
	bool is_daemon()const;
	std::string get_liblogic_path() const;
	std::string get_log_dir()const;
	uint32_t get_log_level()const;
	uint32_t get_log_max_byte()const;
	uint32_t get_log_max_files()const;
	uint32_t get_log_save_next_file_interval_min()const;
private:
	uint32_t m_max_fd_num;//打开文件的最大数量
	bool m_daemon;//是否后台运行	
	std::string m_liblogic_path;//代码段SO路径
	std::string m_log_dir;//日志目录
	uint32_t m_log_level;//日志等级
	uint32_t m_log_max_byte;//日志每个文件的最大大小(字节)
	uint32_t m_log_max_files;//日志文件的最大数量
	uint32_t m_log_save_next_file_interval_min;//每多少时间(分钟)重新保存文件中(新文件)
	//std::string libdata_path;//数据段SO路径//mark 增加在线加载功能
};

extern bench_conf_t g_bench_conf;
