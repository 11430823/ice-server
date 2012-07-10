/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	ok
	brief:		linux version
*********************************************************************/

#pragma once

#include "lib_include.h"

namespace ice{

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36)
	/*......codes under version newer than 2.6.36......*/
#else
	/*......codes under version older than 2.6.36......*/
#endif

}//end namespace ice
