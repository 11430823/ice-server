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
		/**
		* @brief 把getnameinfo、getaddrinfo等函数返回的EAI_XXX错误码转换成类似的EXXX(errno)错误码。
		* @param eai EAI_XXX错误码
		* @return 返回类似的EXXX(errno)错误码
		*/
		static int eai_to_errno(int eai);
		//************************************
		// Brief:	  close the given fd(if fd is VALID),and set to be INVALID_SOCKET 
		// Returns:   int (0 on success, -1 on error)
		// Parameter: int & s (fd)
		//************************************
		static inline int close_socket(int& s);
		/**
		* @brief 用于通过网卡接口（eth0/eth1/lo...）获取对应的IP地址。支持IPv4和IPv6。
		* @param nif 网卡接口。eth0/eth1/lo...
		* @param af 网络地址类型。AF_INET或者AF_INET6。
		* @param ipaddr 用于返回nif和af对应的IP地址。ipaddr的空间由函数调用者分配，并且长度必须大于或者等于IP地址的长度（16或者46字节）。
		* @param len ipaddr的长度（字节）。
		* @return 成功返回0，并且ipaddr中保存了nif和af对应的IP地址。失败返回-1。
		*/
		static int get_ip_addr(const char* nif, int af, void* ipaddr, size_t len);
	protected:
		
	private:
		lib_net_util(const lib_net_util& cr);
		lib_net_util& operator=(const lib_net_util& cr);
	};
	

	class lib_net
	{
	public:
		lib_net();
		virtual ~lib_net();
	protected:
		int fd;
	private:
		lib_net(const lib_net& cr);
		lib_net& operator=(const lib_net& cr);
	};

	class lib_net_multicast
	{
	public:
		lib_net_multicast();
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
