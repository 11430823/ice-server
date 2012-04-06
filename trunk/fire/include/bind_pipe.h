/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		绑定的子进程与父进程之间的PIPE
*********************************************************************/

#pragma once

enum {
	E_PIPE_INDEX_RDONLY = 0,
	E_PIPE_INDEX_WRONLY = 1,
	E_PIPE_INDEX_MAX,
};

#pragma pack(1)

//父子进程之间的管道.
struct bind_pipe_t {
	int handles[E_PIPE_INDEX_MAX];
	bind_pipe_t();
	int create();
};


#pragma pack()