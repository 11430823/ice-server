/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	守护进程配置
	brief:		
*********************************************************************/
#pragma once

#include <string>
#include <vector>

#include "iatomic.h"

class daemon_info_t{
public:
	daemon_info_t();
	~daemon_info_t();
	//************************************
	// Brief:     解析程序传入参数
	// Parameter: int argc
	// Parameter: char * * argv
	//************************************
	void prase_args(int argc, char** argv);
	//************************************
	// Brief:     杀死所有子进程(SIGKILL),并等待其退出后返回.
	// Returns:   void
	//************************************
	void killall_children();
	//程序名称
	std::string prog_name;
	//当前目录
	std::string current_dir;
	//true:停止.false:继续(无改变)
	volatile bool stop;
	//true:重启.false:继续(无改变)
	volatile bool restart;
	

	//子进程ID
	std::vector<atomic_t> child_pids;
};

extern daemon_info_t g_daemon;




