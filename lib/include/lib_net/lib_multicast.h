/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		组播
*********************************************************************/

#pragma once

#include "lib_udp.h"

namespace ice{

#pragma pack(1)
	struct mcast_pkg_header_t {
		uint16_t	pkg_type;   // for mcast_notify_addr: 1st, syn
		uint16_t	proto_type; // mcast_notify_addr...
		char		body[];
		mcast_pkg_header_t();
	};
#pragma pack()

	class lib_multicast_t : public lib_udp_t
	{
		PROTECTED_RW_DEFAULT(std::string, mcast_incoming_if);//组播 接收
		PROTECTED_RW_DEFAULT(std::string, mcast_outgoing_if);//组播 发送
		PROTECTED_RW_DEFAULT(std::string, mcast_ip);//239.X.X.X组播地址
		PROTECTED_RW_DEFAULT(uint16_t, mcast_port);//239.X.X.X组播 端口
	public:
		lib_multicast_t(){
			this->mcast_port = 0;
		}
		virtual ~lib_multicast_t(){}

		/**
		* @brief	创建组播
		* @param	const std::string & mcast_ip 组播地址
		* @param	uint16_t mcast_port 组播端口
		* @param	const std::string & mcast_incoming_if 接收
		* @param	const std::string & mcast_outgoing_if 发送
		* @return	int 0:成功
		*/
		int create(const std::string& mcast_ip, uint16_t mcast_port,
			const std::string& mcast_incoming_if, const std::string& mcast_outgoing_if);
	protected:
	private:
		lib_multicast_t(const lib_multicast_t& cr);
		lib_multicast_t& operator=(const lib_multicast_t& cr);
	private:
	};

	class lib_addr_multicast_t : public lib_multicast_t
	{
	public:
		enum {
			MCAST_NOTIFY_ADDR   = 0,
		};

		enum E_ADDR_MCAST_PKG_TYPE{
			ADDR_MCAST_1ST_PKG	= 1,//启动时第一次发包
			ADDR_MCAST_SYN_PKG	= 2//平时同步用包
		};
#pragma pack(1)
		struct addr_mcast_pkg_t {
			uint32_t	svr_id;
			char		name[16];
			char		ip[16];
			uint16_t	port;
			addr_mcast_pkg_t();
		};
#pragma pack()
	public:
		lib_addr_multicast_t(){}
		virtual ~lib_addr_multicast_t(){}
		void mcast_notify_addr(uint32_t svr_id, const char* svr_name,
			const char* svr_ip, uint16_t svr_port, E_ADDR_MCAST_PKG_TYPE pkg_type = ADDR_MCAST_SYN_PKG);
	protected:
		
	private:
		lib_addr_multicast_t(const lib_addr_multicast_t& cr);
		lib_addr_multicast_t& operator=(const lib_addr_multicast_t& cr);
		mcast_pkg_header_t hdr;
		addr_mcast_pkg_t   pkg;
	};
	

}//end namespace ice