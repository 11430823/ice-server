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
#include "lib_file.h"

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
		void close(){
			lib_file_t::close_fd(this->fd);
			this->init();
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
		typedef void (*ON_SVR_CONN_CLOSED)(int fd);ON_SVR_CONN_CLOSED on_svr_conn_closed;

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
		typedef int	(*ON_GET_PKG_LEN)(int fd, const void* data, int len, int isparent);ON_GET_PKG_LEN on_get_pkg_len;
		on_functions_tcp_server(){
			this->on_events = 0;
			this->on_cli_pkg = 0;
			this->on_srv_pkg = 0;
			this->on_cli_conn_closed = 0;
			this->on_svr_conn_closed = 0;
			this->on_init = 0;
			this->on_fini = 0;
			this->on_get_pkg_len = 0;
		}
	};

	class lib_tcp_sever_t : public lib_tcp_t
	{
	public:
		typedef bool (*CHECK_RUN)();CHECK_RUN check_run;
		PROTECTED_READONLY_DEFAULT(bool, cli_time_out_sec);//���������ĳ�ʱʱ��(��) 0:����ʱ
		PROTECTED_READONLY_DEFAULT(cli_fd_info_t*, cli_fd_infos);//�����û�����Ϣ
		PROTECTED_READONLY_DEFAULT(int, cli_fd_value_max);//�����ϵ�FD�е����ֵ
		PROTECTED_READONLY_DEFAULT(int, listen_fd);//����FD
	public:
		lib_tcp_sever_t(){
			this->cli_time_out_sec = 0;
			this->cli_fd_infos = NULL;
			this->listen_fd = -1;
			this->cli_fd_value_max = 0;
		}
		virtual ~lib_tcp_sever_t(){
			if (NULL != cli_fd_infos){
				for (int i = 0; i < this->cli_fd_value_max; i++) {
					cli_fd_info_t& cfi = cli_fd_infos[i];
					if (FD_TYPE_UNUSED == cfi.fd_type){
						continue;
					}
					lib_file_t::close_fd(cfi.fd);
				}
				safe_delete_arr(this->cli_fd_infos);
			}
			lib_file_t::close_fd(this->listen_fd);
		}
		virtual int register_on_functions(const on_functions_tcp_server* functions) = 0;
		virtual int create() = 0;
		virtual int listen(const char* ip, uint16_t port, uint32_t listen_num, int bufsize) = 0;
		virtual int run(CHECK_RUN check_run_fn) = 0;
		virtual int add_connect(int fd, E_FD_TYPE fd_type, struct sockaddr_in* peer) = 0;
		/**
		* @brief Accept a TCP connection
		* @param struct sockaddr_in* peer,  used to return the protocol address of the connected peer process.  
		* @param int block,  true and the accepted fd will be set blocking, false and the fd will be set nonblocking.
		* @return int, the accpected fd on success, -1 on error.
		*/
		virtual int accept(struct sockaddr_in& peer, bool block);
		virtual int bind(const char* ip,uint16_t port);
		void set_cli_time_out_sec(uint32_t time_out_sec);
		/**
		* @brief ����һ��TCP listen socket����UDP socket�����ڽ��տͻ������ݡ�֧��IPv4��IPv6��
		* @param host �����ĵ�ַ��������IP��ַ��Ҳ������������������ݲ���Ϊ��0.0.0.0���������INADDR_ANY��
		* @param serv �����Ķ˿ڡ����������ֶ˿ڣ�Ҳ�����Ƕ˿ڶ�Ӧ�ķ���������dns��time��ftp�ȡ�
		* @param backlog δ������Ӷ��еĴ�С��һ����1024���ӡ�
		* @param bufsize socket����/���ͻ����С���ֽڣ�������С��10 * 1024 * 1024��
		* @return �ɹ������´�����fd��ʧ�ܷ���-1��fd����ʱ����ʹ��close�رա�
		* @see safe_socket_listen
		*/
		static int create_passive_endpoint(const char* host, const char* serv, int backlog, int bufsize);

	public:
	private:
		lib_tcp_sever_t(const lib_tcp_sever_t& cr);
		lib_tcp_sever_t& operator=(const lib_tcp_sever_t& cr);
	};

}//end namespace ice