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

#include <lib_iatomic.h>
#include <lib_util.h>

class daemon_t
{
public:
	daemon_t();
	virtual ~daemon_t();
	/**
	 * @brief	解析程序传入参数,设置环境,信号处理
	 * @param	int argc
	 * @param	char * * argv
	 * @return	void
	 */
	void prase_args(int argc, char** argv);
	int run();
	//************************************
	// Brief:     杀死所有子进程(SIGKILL),并等待其退出后返回.
	// Returns:   void
	//************************************
	void killall_children();
	void restart_child_process(struct bind_config_elem_t* bc_elem);
	//true:停止.false:继续(无改变)
	volatile bool stop;
	//true:重启.false:继续(无改变)
	volatile bool restart;
	//子进程ID
	std::vector<atomic_t> child_pids;
	//程序名称
	PROPERTY_READONLY_DEFAULT(std::string, prog_name);
	//当前目录
	PROPERTY_READONLY_DEFAULT(std::string, current_dir);
private:
};

extern bool	g_is_parent;
extern daemon_t g_daemon;




