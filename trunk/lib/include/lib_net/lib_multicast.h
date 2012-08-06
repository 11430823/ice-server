/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	ok
	brief:		组播
*********************************************************************/

#pragma once

#include "lib_udp.h"

namespace ice{
	class lib_mcast_t : public lib_udp_t{
		PROTECTED_RW(std::string, mcast_incoming_if);//组播 接收
		PROTECTED_RW(std::string, mcast_outgoing_if);//组播 发送
		PROTECTED_RW(std::string, mcast_ip);//239.X.X.X组播地址
		PROTECTED_RW(uint16_t, mcast_port);//239.X.X.X组播 端口
	public:
		lib_mcast_t(){
			this->mcast_port = 0;
		}
		virtual ~lib_mcast_t(){}

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
		lib_mcast_t(const lib_mcast_t& cr);
		lib_mcast_t& operator=(const lib_mcast_t& cr);
		int mcast_join();
	private:
	};
}//end namespace ice