/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		�鲥
*********************************************************************/

#pragma once

#include "lib_net.h"

namespace ice{
	class lib_multicast_t : public lib_net_t
	{
		PROTECTED_RW_DEFAULT(std::string, mcast_incoming_if);//�鲥 ����
		PROTECTED_RW_DEFAULT(std::string, mcast_outgoing_if);//�鲥 ����
		PROTECTED_RW_DEFAULT(std::string, mcast_ip);//239.X.X.X�鲥��ַ
		PROTECTED_RW_DEFAULT(uint16_t, mcast_port);//239.X.X.X�鲥 �˿�
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
		* @brief	�����鲥
		* @param	const std::string & mcast_ip �鲥��ַ
		* @param	uint16_t mcast_port �鲥�˿�
		* @param	const std::string & mcast_incoming_if ����
		* @param	const std::string & mcast_outgoing_if ����
		* @return	int 0:�ɹ�
		*/
		int create(const std::string& mcast_ip, uint16_t mcast_port, const std::string& mcast_incoming_if, const std::string& mcast_outgoing_if);
	protected:

	private:
		lib_multicast_t(const lib_multicast_t& cr);
		lib_multicast_t& operator=(const lib_multicast_t& cr);
	private:
		//�˳��鲥//TODO δ����
		int exit_multicast(int s);
		//�ܾ��鲥//TODO δ����
		int refuse_multicast(int s);
	};
}//end namespace ice