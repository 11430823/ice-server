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
	//************************************
	// Brief:     加载配置文件bench.ini
	// Returns:   int(0:正确,其它:错误)
	//************************************
	int load();
	std::string& get_liblogic_path();
private:
	std::string libdata_path;//数据段SO路径
	std::string liblogic_path;//代码段SO路径
};

extern bench_conf_t bench_conf;
