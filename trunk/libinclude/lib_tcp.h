/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		ok
*********************************************************************/

#pragma once

#include <netinet/in.h>

class lib_tcp
{
public:
	lib_tcp();
	//virtual ~lib_tcp();
	//************************************
	// Brief:     Set the given fd to be blocking or noblocking
	// Returns:   int	0 on success, -1 on error.
	// Parameter: int fd	file descriptor to be set.
	// Parameter: bool is_block	true:blocking, false:nonblocking
	//************************************
	static int set_io_block(int fd, bool is_block);
protected:
	
private:
	lib_tcp(const lib_tcp& cr);
	lib_tcp& operator=(const lib_tcp& cr);
};