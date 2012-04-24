/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		×é²¥
*********************************************************************/

#pragma once

namespace ice{
	class lib_multicast_t : public lib_net_t
	{
	public:
		lib_multicast_t();
		virtual ~lib_multicast_t(){}
		virtual int send(const void* buf, int total){}
		virtual int recv(void* buf, int bufsize){}

		int create(const char* ip, uint16_t port);

	protected:

	private:
		lib_multicast_t(const lib_multicast_t& cr);
		lib_multicast_t& operator=(const lib_multicast_t& cr);
	private://TODO
		//¼ÓÈë¶à²¥//TODO Î´²âÊÔ
		int join_multicast(int s);
		//ÍË³ö×é²¥//TODO Î´²âÊÔ
		int exit_multicast(int s);
		//¾Ü¾ø×é²¥//TODO Î´²âÊÔ
		int refuse_multicast(int s);
	};
}//end namespace ice