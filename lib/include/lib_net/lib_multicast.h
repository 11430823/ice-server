/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	ok
	brief:		�鲥
*********************************************************************/

#pragma once

#include "lib_udp.h"

namespace ice{
	class lib_mcast_t : public lib_udp_t{
		PROTECTED_RW(std::string, mcast_incoming_if);//�鲥 ����
		PROTECTED_RW(std::string, mcast_outgoing_if);//�鲥 ����
		PROTECTED_RW(std::string, mcast_ip);//239.X.X.X�鲥��ַ
		PROTECTED_RW(uint16_t, mcast_port);//239.X.X.X�鲥 �˿�
	public:
		lib_mcast_t(){
			this->mcast_port = 0;
		}
		virtual ~lib_mcast_t(){}

		/**
		* @brief	�����鲥
		* @param	const std::string & mcast_ip �鲥��ַ
		* @param	uint16_t mcast_port �鲥�˿�
		* @param	const std::string & mcast_incoming_if ����
		* @param	const std::string & mcast_outgoing_if ����
		* @return	int 0:�ɹ�
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