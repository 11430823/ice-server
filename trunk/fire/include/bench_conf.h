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
	PRIVATE_READONLY_DEFAULT(uint32_t, max_fd_num);//打开文件的最大数量
	PRIVATE_READONLY_DEFAULT(bool, is_daemon);//是否后台运行
	PRIVATE_READONLY_DEFAULT(std::string, liblogic_path);////代码段SO路径
	PRIVATE_READONLY_DEFAULT(uint32_t, log_level);//日志等级
	PRIVATE_READONLY_DEFAULT(time_t, fd_time_out);//连接的FD超时秒数,0:无超时
	PRIVATE_READONLY_DEFAULT(uint32_t, page_size_max);//数据包的最大字节数
	PRIVATE_READONLY_DEFAULT(std::string, log_dir);//日志目录
	PRIVATE_READONLY_DEFAULT(uint32_t, log_save_next_file_interval_min);//每多少时间(分钟)重新保存文件中(新文件)
	PRIVATE_READONLY_DEFAULT(uint32_t, core_size);//core文件的大小，字节
	PRIVATE_READONLY_DEFAULT(uint32_t, restart_cnt_max);//最大重启次数
	PRIVATE_READONLY_DEFAULT(std::string, daemon_tcp_ip);//守护进程地址//默认为0
	PRIVATE_READONLY_DEFAULT(uint16_t, daemon_tcp_port);//守护进程端口//默认为0
public:
	bench_conf_t();
public:
	//so调用获取配置项bench.ini中的数据(自行配置)
	//获取配置项数据
	std::string get_strval(const char* key, const char* name) const;
	//************************************
	// Brief:     加载配置文件bench.ini
	// Returns:   int(0:正确,其它:错误)
	//************************************
	int load();
private:
};

extern bench_conf_t g_bench_conf;
