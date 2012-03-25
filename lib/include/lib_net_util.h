/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		ok
*********************************************************************/

#pragma once

namespace ice{

	class lib_net_util_t
	{
	public:
		/**
		* @brief 把getnameinfo、getaddrinfo等函数返回的EAI_XXX错误码转换成类似的EXXX(errno)错误码。
		* @param eai EAI_XXX错误码
		* @return 返回类似的EXXX(errno)错误码
		* todo 用途?
		*/
		static int eai_to_errno(int eai);
		/**
		* @brief 用于通过网卡接口（eth0/eth1/lo...）获取对应的IP地址。支持IPv4和IPv6。
		* @param nif 网卡接口。eth0/eth1/lo...
		* @param af 网络地址类型。AF_INET或者AF_INET6。
		* @param ipaddr 用于返回nif和af对应的IP地址。ipaddr的空间由函数调用者分配，并且长度必须大于或者等于IP地址的长度（16或者46字节）。
		* @param len ipaddr的长度（字节）。
		* @return 成功返回0，并且ipaddr中保存了nif和af对应的IP地址。失败返回-1。
		* todo 用途?
		*/
		static int get_ip_addr(const char* nif, int af, void* ipaddr, size_t len);
	protected:
		
	private:
		lib_net_util_t(const lib_net_util_t& cr);
		lib_net_util_t& operator=(const lib_net_util_t& cr);
	};
	

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

	class lib_net_multicast_t
	{
	public:
		lib_net_multicast_t();
		virtual ~lib_net_multicast_t(){}
		//加入多播//TODO 未测试
		int join_multicast(int s);
		//退出组播//TODO 未测试
		int exit_multicast(int s);
		//拒绝组播//TODO 未测试
		int refuse_multicast(int s);
	protected:
		
	private:
		lib_net_multicast_t(const lib_net_multicast_t& cr);
		lib_net_multicast_t& operator=(const lib_net_multicast_t& cr);
	};

}//end namespace ice
