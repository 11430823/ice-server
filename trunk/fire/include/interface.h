/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		�û��ӿ�(��ʱ����)
*********************************************************************/

#pragma once

#include <stdint.h>

namespace fire{
	int send(int fd, const void* data, uint32_t len);
}//end namespace fire