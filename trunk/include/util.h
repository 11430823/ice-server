/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		
*********************************************************************/

#pragma once
#include <stdint.h>
#include "log.h"

#ifdef  likely
#undef  likely
#endif
#define likely(x)  __builtin_expect(!!(x), 1)

#ifdef  unlikely
#undef  unlikely
#endif
#define unlikely(x)  __builtin_expect(!!(x), 0)

#define FOREACH(container,it) \
	for(typeof((container).begin()) it = (container).begin();it!=(container).end();++it)
int log_init_ex(const char* dir, log_lvl_t lvl, uint32_t size, int maxfiles, const char* pre_name, uint32_t logtime);