/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		
*********************************************************************/

#pragma once

#ifdef  likely
#undef  likely
#endif
#define likely(x)  __builtin_expect(!!(x), 1)

#ifdef  unlikely
#undef  unlikely
#endif
#define unlikely(x)  __builtin_expect(!!(x), 0)
