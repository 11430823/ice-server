/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	todo
	brief:		网络杂项
*********************************************************************/

#pragma once

#include "lib_include.h"

namespace ice{
	enum E_FD_TYPE{
		FD_TYPE_UNUSED = 0,
		FD_TYPE_LISTEN = 1,
		FD_TYPE_PIPE = 2,
		FD_TYPE_CLI = 3,
		FD_TYPE_MCAST = 4,
		FD_TYPE_ADDR_MCAST = 5,
		FD_TYPE_UDP = 6,
		FD_TYPE_ASYN_CONNECT = 7,
		FD_TYPE_SVR = 8,
	};

	class lib_net_util_t{
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

		//todo test
		static bool get_local_ip( std::string& ip);

		static bool get_local_ip( long& ip);

		/**
		* @brief	translate the given address family to its corresponding level
		*/
		static int family_to_level(int family);
		/**
		* @brief Set a timeout on sending data. If you want to disable timeout, just simply
		*           call this function again with millisec set to be 0.
		*
		* @param sockfd socket descriptor to be set.
		* @param millisec timeout in milliseconds.
		*
		* @return 0 on success, -1 on error and errno is set appropriately.
		*/
		static inline int set_sock_send_timeo(int sockfd, int millisec){
			struct timeval tv;

			tv.tv_sec  = millisec / 1000;
			tv.tv_usec = (millisec % 1000) * 1000;

			return ::setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
		}

		/**
		* @brief Set a timeout on receiving data. If you want to disable timeout, just simply
		*           call this function again with millisec set to be 0.
		*
		* @param sockfd socket descriptor to be set.
		* @param millisec timeout in milliseconds.
		*
		* @return 0 on success, -1 on error and errno is set appropriately.
		*/
		static inline int set_sock_rcv_timeo(int sockfd, int millisec){
			struct timeval tv;

			tv.tv_sec  = millisec / 1000;
			tv.tv_usec = (millisec % 1000) * 1000;

			return ::setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
		}
		//************************************
		// Brief:	  Set the given fd recv buffer
		// Returns:   int (0 on success, -1 on error)
		// Parameter: int s (fd)
		// Parameter: uint32_t len (recv buffer len)
		//************************************
		static inline int set_recvbuf(int s, uint32_t len){
			return ::setsockopt(s, SOL_SOCKET, SO_RCVBUF, &len, sizeof(len));
		}
		static inline int set_sendbuf(int s, uint32_t len){
			return ::setsockopt(s, SOL_SOCKET, SO_SNDBUF, &len, sizeof(len));
		}
		static inline uint32_t ip2int(const char* ip){
			return inet_addr(ip);
		}
		static inline std::string ip2str(uint32_t ip){
			struct in_addr a;
			a.s_addr = ip;
			std::string s = inet_ntoa(a);
			return s;
		}
	protected:
		
	private:
		lib_net_util_t(const lib_net_util_t& cr);
		lib_net_util_t& operator=(const lib_net_util_t& cr);
	};
}//end namespace ice
