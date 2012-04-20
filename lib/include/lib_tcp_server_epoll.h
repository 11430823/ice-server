/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		tcp server by epoll
*********************************************************************/

#pragma once

#include <sys/epoll.h>

#include "lib_tcp_server.h"

namespace ice{
	struct on_functions_tcp_server_epoll : public on_functions_tcp_server 
	{
	};
	class lib_tcp_server_epoll_t : public lib_tcp_sever_t
	{
	public:
		typedef int (*ON_PIPE_EVENT)(int fd, epoll_event& r_evs);
		PRIVATE_RW_DEFAULT(ON_PIPE_EVENT, on_pipe_event);
		PRIVATE_RW_DEFAULT(int, epoll_wait_time_out);//epoll_wait函数调用时超时时间间隔
		PRIVATE_R_DEFAULT(on_functions_tcp_server_epoll*, on_functions);//回调函数
	public:
		lib_tcp_server_epoll_t(uint32_t max_events_num);
		virtual ~lib_tcp_server_epoll_t();
		virtual void register_on_functions(const on_functions_tcp_server* functions);
		virtual int create();
		/**
		* @brief Create a listening socket fd
		* @param ip the binding ip address. If this argument is assigned with 0,
		*                                           then INADDR_ANY will be used as the binding address.
		* @param port the binding port.
		* @param listen_num the maximum length to which the queue of pending connections for sockfd may grow.
		* @param bufsize maximum socket send and receive buffer in bytes, should be less than 10 * 1024 * 1024
		* @return the newly created listening fd on success, -1 on error.
		* @see create_passive_endpoint
		*/
		virtual int listen(const char* ip, uint16_t port, uint32_t listen_num, int bufsize);
		virtual int run(CHECK_RUN check_run_fn);
		virtual int add_connect(int fd, E_FD_TYPE fd_type, struct sockaddr_in* peer);
	public:
		int mod_events(int fd, uint32_t flag);
		int add_events(int fd, uint32_t flag);
	private:
		lib_tcp_server_epoll_t(const lib_tcp_server_epoll_t& cr);
		lib_tcp_server_epoll_t& operator=(const lib_tcp_server_epoll_t& cr);

		void handle_client(lib_tcp_client_t& fd_info);
		void handle_listen();
		int handle_send(lib_tcp_client_t& fd_info);
	};
}//end namespace ice