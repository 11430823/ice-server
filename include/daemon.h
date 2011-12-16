/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	守护进程配置
	brief:		ok
*********************************************************************/
#pragma once

#include <string>
#include <vector>

#include <ice_lib/iatomic.h>

class daemon_t
{
public:
	daemon_t();
	~daemon_t();
	//************************************
	// Brief:     解析程序传入参数,设置环境,信号处理
	// Parameter: int argc
	// Parameter: char * * argv
	//************************************
	void prase_args(int argc, char** argv);
	//************************************
	// Brief:     杀死所有子进程(SIGKILL),并等待其退出后返回.
	// Returns:   void
	//************************************
	void killall_children();
	//true:停止.false:继续(无改变)
	volatile bool m_stop;
	//true:重启.false:继续(无改变)
	volatile bool m_restart;
	//子进程ID
	std::vector<atomic_t> child_pids;
private:
	//程序名称
	std::string m_prog_name;
	//当前目录
	std::string m_current_dir;
};

extern daemon_t g_daemon;




