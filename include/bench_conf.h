/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	读取bench.ini配置文件
	brief:		ok
*********************************************************************/
#pragma once

#include <string>

#include <lib_util.h>

class bench_conf_t 
{
public:
//so调用获取配置项bench.ini中的数据(自行配置)
	//获取配置项数据
	std::string get_strval(std::string& key, std::string& name) const;
public:
	bench_conf_t();
	//************************************
	// Brief:     加载配置文件bench.ini
	// Returns:   int(0:正确,其它:错误)
	//************************************
	int load();
	PROPERTY_READONLY_DEFAULT(uint32_t, m_max_fd_num);//打开文件的最大数量
	PROPERTY_READONLY_DEFAULT(bool, m_daemon);//是否后台运行
	PROPERTY_READONLY_DEFAULT(std::string, m_liblogic_path);////代码段SO路径
	PROPERTY_READONLY_DEFAULT(uint32_t, m_log_level);//日志等级
	PROPERTY_READONLY_DEFAULT(time_t, m_fd_time_out);//连接的FD超时秒数,0:无超时
	PROPERTY_READONLY_DEFAULT(uint32_t, m_page_size_max);//数据包的最大字节数
	PROPERTY_READONLY_DEFAULT(std::string, m_log_dir);//日志目录
	PROPERTY_READONLY_DEFAULT(uint32_t, m_log_save_next_file_interval_min);//每多少时间(分钟)重新保存文件中(新文件)
	PROPERTY_READONLY_DEFAULT(uint32_t, m_core_size);//core文件的大小，字节
private:
	//std::string libdata_path;//数据段SO路径//mark 增加在线加载功能
};

extern bench_conf_t g_bench_conf;
