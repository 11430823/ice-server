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

#pragma pack(1)
	struct mcast_pkg_header_t {
		uint16_t	pkg_type;   // for mcast_notify_addr: 1st, syn
		uint16_t	proto_type; // mcast_notify_addr...
		char		body[];
		mcast_pkg_header_t();
	};
#pragma pack()

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
			//todo man sendto �鿴��ʲô��Ҫע���
			return ::sendto(this->fd, buf, total, 0, (sockaddr*)&(this->mcast_addr), sizeof(this->mcast_addr));
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
		sockaddr_in mcast_addr;
	private:
		lib_multicast_t(const lib_multicast_t& cr);
		lib_multicast_t& operator=(const lib_multicast_t& cr);
	private:
		
	};

	class lib_addr_multicast_t : public lib_multicast_t
	{
		enum {
			MCAST_NOTIFY_ADDR   = 0,
		};

		enum E_ADDR_MCAST_PKG_TYPE{
			ADDR_MCAST_1ST_PKG	= 1,
			ADDR_MCAST_SYN_PKG	= 2
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
		void pack_this_service_info(uint32_t svr_id, const char* svr_name,
			const char* svr_ip, uint16_t svr_port, E_ADDR_MCAST_PKG_TYPE pkg_type = ADDR_MCAST_SYN_PKG);
	protected:
		
	private:
		lib_addr_multicast_t(const lib_addr_multicast_t& cr);
		lib_addr_multicast_t& operator=(const lib_addr_multicast_t& cr);
		mcast_pkg_header_t hdr;
		addr_mcast_pkg_t   pkg;
	};
	

}//end namespace ice