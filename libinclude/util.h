/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		
*********************************************************************/

#pragma once
#include <stdint.h>
#include <iostream>
#include <sstream>
#include <string>

#ifdef  likely
#undef  likely
#endif
#define likely(x)  __builtin_expect(!!(x), 1)

#ifdef  unlikely
#undef  unlikely
#endif
#define unlikely(x)  __builtin_expect(!!(x), 0)

#define FOREACH(container,it) \
	for(typeof((container).begin()) it = (container).begin();(it)!=(container).end();++(it))

template <class T> 
void g_convert_from_string(T &value, const std::string &s) {
	std::stringstream ss(s);
	ss >> value;
}