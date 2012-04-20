/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		用户接口(暂时无用)
*********************************************************************/

#pragma once

#include <stdint.h>

namespace fire{
	int send(int fd, const void* data, uint32_t len);
}//end namespace fire