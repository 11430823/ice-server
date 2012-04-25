/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		组播
*********************************************************************/

#pragma once

#include "lib_net.h"

namespace ice{
	class lib_multicast_t : public lib_net_t
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
		virtual int send(const void* buf, int total){
			return 0;
		}
		virtual int recv(void* buf, int bufsize){
			return 0;
		}

		/**
		* @brief	创建组播
		* @param	const std::string & mcast_ip 组播地址
		* @param	uint16_t mcast_port 组播端口
		* @param	const std::string & mcast_incoming_if 接收
		* @param	const std::string & mcast_outgoing_if 发送
		* @return	int 0:成功
		*/
		int create(const std::string& mcast_ip, uint16_t mcast_port, const std::string& mcast_incoming_if, const std::string& mcast_outgoing_if);
	protected:

	private:
		lib_multicast_t(const lib_multicast_t& cr);
		lib_multicast_t& operator=(const lib_multicast_t& cr);
	private:
		//退出组播//TODO 未测试
		int exit_multicast(int s);
		//拒绝组播//TODO 未测试
		int refuse_multicast(int s);
	};
}//end namespace ice