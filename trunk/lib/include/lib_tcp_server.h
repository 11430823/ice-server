/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		tcp server
*********************************************************************/

#pragma once

#include <stdint.h>

#include "lib_tcp.h"
#include "lib_util.h"

namespace ice{
	enum E_FD_TYPE{
		FD_TYPE_UNUSED = 0,
		FD_TYPE_LISTEN = 1,
		FD_TYPE_PIPE = 2,
		FD_TYPE_REMOTE = 3,
		FD_TYPE_MCAST = 4,
		FD_TYPE_ADDR_MCAST = 5,
		FD_TYPE_UDP = 6,
		FD_TYPE_ASYN_CONNECT = 7,
	};
	#pragma pack(1)
	struct cli_fd_info_t{
		int			fd;
		uint8_t		fd_type;
		uint16_t	remote_port;
		uint32_t	remote_ip;
		uint32_t	last_tm;
		cli_fd_info_t(){
			this->init();
		}
		void init(){
			this->fd = -1;
			this->fd_type = FD_TYPE_UNUSED;
			this->remote_port = 0;
			this->remote_ip = 0;
			this->last_tm = 0;
		}
	};
	#pragma pack()
	
	//�������ص�����
	struct on_functions_tcp_server
	{
	//The following interfaces are called only by the child process
		//************************************
		// Brief:	Called each time before processing packages from clients.
		//			Calling interval of this interface is no much longer than 100ms at maximum.
		//************************************
		typedef void (*ON_EVENTS)();ON_EVENTS on_events;
		//************************************
		// Brief:  	Called to process packages from clients. Called once for each package.
		// 			Return non-zero if you want to close the client connection from which the `pkg` is sent,
		// 			otherwise returns 0. If non-zero is returned, `on_cli_conn_closed` will be called too. 
		//************************************
		typedef int (*ON_CLI_PKG)(void* pkg, int pkglen, cli_fd_info_t* cli_fd_info);ON_CLI_PKG on_cli_pkg;
		//************************************
		// Brief:	Called to process packages from servers that the child connects to. Called once for each package.
		//************************************
		typedef void (*ON_SRV_PKG)(int fd, void* pkg, int pkglen);ON_SRV_PKG on_srv_pkg;
		//************************************
		// Brief:	Called each time when a client close a connection, or when `on_cli_pkg` returns non-zero.
		//************************************
		typedef void (*ON_CLI_CONN_CLOSED)(int fd);ON_CLI_CONN_CLOSED on_cli_conn_closed;
		//************************************
		// Brief:	Called each time on close of the FDs opened by the child.
		//************************************
		typedef void (*ON_FD_CLOSED)(int fd);ON_FD_CLOSED on_fd_closed;

	//The following interfaces are called both by the parent and child process
		//************************************
		// Brief:	//Called only once at server startup by both the parent and child process.
					//`isparent == 1` indicates this interface is called by the parent;
					//`isparent == 0` indicates this interface is called by the child.
					//You should initialize your service program (allocate memory, create objects, etc) here.
					//You must return 0 on success, -1 otherwise.
		//************************************
		typedef int (*ON_INIT)(int isparent);ON_INIT on_init;
		//************************************
		// Brief:	//Called only once at server stop by both the parent and child process.
					//`isparent == 1` indicates this interface is called by the parent;
					//`isparent == 0` indicates this interface is called by the child.
					//You should finalize your service program (release memory, destroy objects, etc) here.
					//You must return 0 if you have finished finalizing the service, -1 otherwise. 
		//************************************
		typedef int (*ON_FINI)(int isparent);ON_FINI on_fini;
		/*!
		  * This interface will be called both by the parent and child process.
		  * `isparent == 1` indicates this interface is called by the parent;
		  * `isparent == 0` indicates this interface is called by the child.
		  * You must return 0 if you cannot yet determine the length of the incoming package,
		  * return -1 if you find that the incoming package is invalid and ice-server will close the connection,
		  * otherwise, return the length of the incoming package. Note, the package should be no larger than 8192 bytes.
		  */
		typedef int	(*ON_GET_PKG_LEN)(int fd, const void* avail_data, int avail_len, int isparent);ON_GET_PKG_LEN on_get_pkg_len;
		on_functions_tcp_server(){
			this->on_events = 0;
			this->on_cli_pkg = 0;
			this->on_srv_pkg = 0;
			this->on_cli_conn_closed = 0;
			this->on_fd_closed = 0;
			this->on_init = 0;
			this->on_fini = 0;
			this->on_get_pkg_len = 0;
		}
	};

	class lib_tcp_sever_t : public lib_tcp_t
	{
		PROTECTED_READONLY_DEFAULT(bool, is_run);
		PROTECTED_READONLY_DEFAULT(bool, cli_time_out_sec);//���������ĳ�ʱʱ��(��) 0:����ʱ
		PROTECTED_READONLY_DEFAULT(cli_fd_info_t*, cli_fd_infos);//�����û�����Ϣ
		PROTECTED_READONLY_DEFAULT(int, cli_fd_value_max);//�����ϵ�FD�е����ֵ
	public:
		lib_tcp_sever_t(){
			this->is_run = true;
			this->cli_time_out_sec = 0;
			this->cli_fd_infos = NULL;
		}
		virtual ~lib_tcp_sever_t(){
		}
		virtual int register_on_functions(const on_functions_tcp_server* functions) = 0;
		virtual int create() = 0;
		virtual int listen(uint32_t listen_num) = 0;
		virtual int run() = 0;
		virtual int stop() = 0;
		virtual int add_connect(int fd, E_FD_TYPE fd_type, struct sockaddr_in* peer) = 0;
		void set_cli_time_out_sec(uint32_t time_out_sec);
		/**
		* @brief Create a listening socket fd
		*
		* @param ipaddr the binding ip address. If this argument is assigned with 0,
		*                                           then INADDR_ANY will be used as the binding address.
		* @param port the binding port.
		* @param backlog the maximum length to which the queue of pending connections for sockfd may grow.
		* @param bufsize maximum socket send and receive buffer in bytes, should be less than 10 * 1024 * 1024
		*
		* @return the newly created listening fd on success, -1 on error.
		* @see create_passive_endpoint
		*/
		static int safe_socket_listen(const char* ipaddr, in_port_t port, int backlog, int bufsize);
		/**
		* @brief ����һ��TCP listen socket����UDP socket�����ڽ��տͻ������ݡ�֧��IPv4��IPv6��
		*
		* @param host �����ĵ�ַ��������IP��ַ��Ҳ������������������ݲ���Ϊ��0.0.0.0���������INADDR_ANY��
		* @param serv �����Ķ˿ڡ����������ֶ˿ڣ�Ҳ�����Ƕ˿ڶ�Ӧ�ķ���������dns��time��ftp�ȡ�
		* @param backlog δ������Ӷ��еĴ�С��һ����1024���ӡ�
		* @param bufsize socket����/���ͻ����С���ֽڣ�������С��10 * 1024 * 1024��
		*
		* @return �ɹ������´�����fd��ʧ�ܷ���-1��fd����ʱ����ʹ��close�رա�
		* @see safe_socket_listen
		*/
		static int create_passive_endpoint(const char* host, const char* serv, int backlog, int bufsize);
		/**
		* @brief Accept a TCP connection
		*
		* @param int sockfd,  the listening fd.
		* @param struct sockaddr_in* peer,  used to return the protocol address of the connected peer process.  
		* @param int block,  true and the accepted fd will be set blocking, false and the fd will be set nonblocking.
		*
		* @return int, the accpected fd on success, -1 on error.
		*/
		static int safe_tcp_accept(int sockfd, struct sockaddr_in* peer, bool block);
	public:
		void update_cli_fd_value_max_by_add(int fd){
			if (this->cli_fd_value_max < fd){
				this->cli_fd_value_max = fd;
			}
		}
		void update_cli_fd_value_max_by_reduce(int fd){
			//todo ��ô����?
		}

	private:
		lib_tcp_sever_t(const lib_tcp_sever_t& cr);
		lib_tcp_sever_t& operator=(const lib_tcp_sever_t& cr);
	};

}//end namespace ice