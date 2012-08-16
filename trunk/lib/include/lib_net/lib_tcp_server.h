/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	todo
	brief:		tcp server
*********************************************************************/

#pragma once

#include "lib_include.h"
#include "lib_tcp.h"
#include "lib_util.h"
#include "lib_file.h"
#include "lib_log.h"
#include "lib_tcp_client.h"

namespace ice{
	
	//�������ص�����
	struct on_functions_tcp_srv
	{
	//The following interfaces are called only by the child process
		//************************************
		// Brief:	Called each time before processing packages from clients.
		//			Calling interval of this interface is no much longer than 100ms at maximum.
		//************************************
		typedef void (*ON_EVENTS)();
		ON_EVENTS on_events;
		//************************************
		// Brief:  	Called to process packages from clients. Called once for each package.
		// 			Return non-zero if you want to close the client connection from which the `pkg` is sent,
		// 			otherwise returns 0. If non-zero is returned, `on_cli_conn_closed` will be called too. 
		//************************************
		typedef int (*ON_CLI_PKG)(const void* pkg, int pkglen, ice::lib_tcp_peer_info_t* peer_fd_info);
		ON_CLI_PKG on_cli_pkg;
		//************************************
		// Brief:	Called to process packages from servers that the child connects to. Called once for each package.
		//************************************
		typedef void (*ON_SRV_PKG)(const void* pkg, int pkglen, ice::lib_tcp_peer_info_t* peer_fd_info);
		ON_SRV_PKG on_srv_pkg;
		//************************************
		// Brief:	Called each time when a client close a connection, or when `on_cli_pkg` returns non-zero.
		//************************************
		typedef void (*ON_CLI_CONN_CLOSED)(int fd);
		ON_CLI_CONN_CLOSED on_cli_conn_closed;
		//************************************
		// Brief:	Called each time on close of the FDs opened by the child.
		//************************************
		typedef void (*ON_SVR_CONN_CLOSED)(int fd);
		ON_SVR_CONN_CLOSED on_svr_conn_closed;

	//The following interfaces are called both by the parent and child process
		//************************************
		// Brief:	//Called only once at server startup by both the parent and child process.
					//You should initialize your service program (allocate memory, create objects, etc) here.
					//You must return 0 on success, -1 otherwise.
		//************************************
		typedef int (*ON_INIT)();
		ON_INIT on_init;
		//************************************
		// Brief:	//Called only once at server stop by both the parent and child process.
					//You should finalize your service program (release memory, destroy objects, etc) here.
					//You must return 0 if you have finished finalizing the service, -1 otherwise. 
		//************************************
		typedef int (*ON_FINI)();
		ON_FINI on_fini;
		/*!
		  * This interface will be called both by the parent and child process.
		  * You must return 0 if you cannot yet determine the length of the incoming package,
		  * return -1 if you find that the incoming package is invalid and ice-server will close the connection,
		  * otherwise, return the length of the incoming package. Note, the package should be no larger than 8192 bytes.
		  */
		typedef int	(*ON_GET_PKG_LEN)(ice::lib_tcp_peer_info_t* peer_fd_info, const void* data, uint32_t len);
		ON_GET_PKG_LEN on_get_pkg_len;

		/*! Called to process multicast packages from the specified `mcast_ip` and `mcast_port`. Called once for each package. */
		typedef void (*ON_MCAST_PKG)(const void* pkg, int len);
		ON_MCAST_PKG on_mcast_pkg;

		/*! Called to process multicast packages from the specified `addr_mcast_ip` and `addr_mcast_port`. Called once for each package. */
		typedef void (*ON_ADDR_MCAST_PKG)(uint32_t id, const char* name, const char* ip, uint16_t port, int flag/*1:����.0:������*/);
		ON_ADDR_MCAST_PKG on_addr_mcast_pkg;

		typedef void (*ON_UDP_PKG)(int fd, const void* data, int len ,struct sockaddr_in* from, socklen_t fromlen);
		ON_UDP_PKG on_udp_pkg;
		on_functions_tcp_srv();
	};

	class lib_tcp_srv_t : public lib_tcp_t
	{
	public:
		typedef bool (*CHECK_RUN)();
		PROTECTED_R(CHECK_RUN, check_run);//������ѭ��ʱ����Ƿ����ִ��
		PROTECTED_RW(uint32_t, cli_time_out_sec);//���������ĳ�ʱʱ��(��) 0:����ʱ
		PROTECTED_R(lib_tcp_peer_info_t*, peer_fd_infos);//�����û�����Ϣ
		PROTECTED_R(int, cli_fd_value_max);//�����ϵ�FD�е����ֵ
		PROTECTED_R(int, listen_fd);//����FD
	public:
		lib_tcp_srv_t();
		virtual ~lib_tcp_srv_t();
		/**
		* @brief	���÷������ص���Ϣ
		*/
		virtual void register_on_functions(const on_functions_tcp_srv* functions) = 0;
		virtual int create() = 0;
		virtual int listen(const char* ip, uint16_t port, uint32_t listen_num, int bufsize) = 0;
		virtual int run(CHECK_RUN check_run_fn) = 0;
		virtual lib_tcp_peer_info_t* add_connect(int fd, E_FD_TYPE fd_type, const char* ip, uint16_t port) = 0;
		/**
		* @brief Accept a TCP connection
		* @param struct sockaddr_in* peer,  used to return the protocol address of the connected peer process.  
		* @param int block,  true and the accepted fd will be set blocking, false and the fd will be set nonblocking.
		* @return int, the accpected fd on success, -1 on error.
		*/
		virtual int accept(struct sockaddr_in& peer, bool block);
	public:
		/** todo
		* @brief ����һ��TCP listen socket����UDP socket�����ڽ��տͻ������ݡ�֧��IPv4��IPv6��
		* @param host �����ĵ�ַ��������IP��ַ��Ҳ������������������ݲ���Ϊ��0.0.0.0���������INADDR_ANY��
		* @param serv �����Ķ˿ڡ����������ֶ˿ڣ�Ҳ�����Ƕ˿ڶ�Ӧ�ķ���������dns��time��ftp�ȡ�
		* @param backlog δ������Ӷ��еĴ�С��һ����1024���ӡ�
		* @param bufsize socket����/���ͻ����С���ֽڣ�������С��10 * 1024 * 1024��
		* @return �ɹ������´�����fd��ʧ�ܷ���-1��fd����ʱ����ʹ��close�رա�
		* @see safe_socket_listen
		*/
		static int create_passive_endpoint(const char* host, const char* serv, int backlog, int bufsize);
	protected:
		virtual int bind(const char* ip,uint16_t port);
	private:
		lib_tcp_srv_t(const lib_tcp_srv_t& cr);
		lib_tcp_srv_t& operator=(const lib_tcp_srv_t& cr);
	};
}//end namespace ice