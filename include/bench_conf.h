/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	��ȡbench.ini�����ļ�
	brief:		
*********************************************************************/
#pragma once

#include <string>

class bench_conf_t 
{
public:
	//************************************
	// Brief:     ���������ļ�bench.ini
	// Returns:   int(0:��ȷ,����:����)
	//************************************
	int load();
	std::string& get_liblogic_path();
private:
	std::string libdata_path;//���ݶ�SO·��
	std::string liblogic_path;//�����SO·��
};

extern bench_conf_t bench_conf;
