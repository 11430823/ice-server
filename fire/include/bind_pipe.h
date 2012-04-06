/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		�󶨵��ӽ����븸����֮���PIPE
*********************************************************************/

#pragma once

enum {
	E_PIPE_INDEX_RDONLY = 0,
	E_PIPE_INDEX_WRONLY = 1,
	E_PIPE_INDEX_MAX,
};

#pragma pack(1)

//���ӽ���֮��Ĺܵ�.
struct bind_pipe_t {
	int handles[E_PIPE_INDEX_MAX];
	bind_pipe_t();
	int create();
};


#pragma pack()