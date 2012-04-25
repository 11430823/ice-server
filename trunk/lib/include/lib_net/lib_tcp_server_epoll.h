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
	struct on_functions_tcp_server_epoll : public on_functions_tcp_srv 
	{
	};
	class lib_tcp_server_epoll_t : public lib_tcp_srv_t
	{
	public:
		typedef int (*ON_PIPE_EVENT)(int fd, epoll_event& r_evs);
		PRIVATE_RW_DEFAULT(ON_PIPE_EVENT, on_pipe_event);
		PRIVATE_RW_DEFAULT(int, epoll_wait_time_out);//epoll_wait函数调用时超时时间间隔
		PRIVATE_R_DEFAULT(on_functions_tcp_server_epoll*, on_functions);//回调函数
	public:
		lib_tcp_server_epoll_t(uint32_t max_events_num);
		virtual ~lib_tcp_server_epoll_t();
		virtual void register_on_functions(const on_functions_tcp_srv* functions);
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
		virtual lib_tcp_peer_info_t* add_connect(int fd, E_FD_TYPE fd_type, const char* ip, uint16_t port);
	public:
		int mod_events(int fd, uint32_t flag);
		int add_events(int fd, uint32_t flag);
		/**
		 * @brief	关闭连接
		 * @param	bool do_calback true:调用客户端注册的回调函数.通知被关闭, false:不做通知
		 */
		void close_peer(lib_tcp_peer_info_t& fd_info, bool do_calback = true);
	private:
		lib_tcp_server_epoll_t(const lib_tcp_server_epoll_t& cr);
		lib_tcp_server_epoll_t& operator=(const lib_tcp_server_epoll_t& cr);

		void handle_peer_msg(lib_tcp_peer_info_t& fd_info);
		void handle_peer_mcast_msg(lib_tcp_peer_info_t& fd_info);
		void handle_listen();
		int handle_send(lib_tcp_peer_info_t& fd_info);
	};
}//end namespace ice