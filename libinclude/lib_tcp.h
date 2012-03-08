/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		ok
*********************************************************************/

#pragma once

#include <netinet/in.h>
#include <lib_util.h>

namespace ice{
	class lib_tcp_t
	{
		PROPERTY_READONLY_DEFAULT(int, m_fd);
	public:
		lib_tcp_t();
		//virtual ~lib_tcp_t();
	protected:
		
	private:
		lib_tcp_t(const lib_tcp_t& cr);
		lib_tcp_t& operator=(const lib_tcp_t& cr);
	};	
}//end namespace ice
