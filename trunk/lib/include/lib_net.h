/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		
*********************************************************************/

#pragma once

namespace ice{
	class lib_net_t
	{
	public:
		lib_net_t();
		virtual ~lib_net_t();
	protected:
		int fd;
	private:
		lib_net_t(const lib_net_t& cr);
		lib_net_t& operator=(const lib_net_t& cr);
	};
}//end namespace ice