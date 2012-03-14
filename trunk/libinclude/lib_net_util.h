/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		ok
*********************************************************************/

#pragma once

namespace ice{

	class lib_net_util
	{
	public:
		lib_net_util(){}
		//virtual ~lib_net_util(){}
		/**
		* @brief 把getnameinfo、getaddrinfo等函数返回的EAI_XXX错误码转换成类似的EXXX(errno)错误码。
		* @param eai EAI_XXX错误码
		* @return 返回类似的EXXX(errno)错误码
		*/
		static int eai_to_errno(int eai);
	protected:
		
	private:
		lib_net_util(const lib_net_util& cr);
		lib_net_util& operator=(const lib_net_util& cr);
	};
	

	class lib_net
	{
	public:
		lib_net(){
			this->fd = -1;
		}
		//virtual ~lib_net(){}
		//************************************
		// Brief:	  close the given fd(if fd is VALID),and set to be INVALID_SOCKET 
		// Returns:   int (0 on success, -1 on error)
		// Parameter: int & s (fd)
		//************************************
		int close_socket(int& s);
	protected:
		int fd;
	private:
		lib_net(const lib_net& cr);
		lib_net& operator=(const lib_net& cr);
	};

	class lib_net_multicast
	{
	public:
		lib_net_multicast(){}
		//virtual ~lib_net_mcast(){}
		//加入多播//TODO 未测试
		int join_multicast(int s);
		//退出组播//TODO 未测试
		int exit_multicast(int s);
		//拒绝组播//TODO 未测试
		int refuse_multicast(int s);
	protected:
		
	private:
		lib_net_multicast(const lib_net_multicast& cr);
		lib_net_multicast& operator=(const lib_net_multicast& cr);
	};
	



}//end namespace ice
