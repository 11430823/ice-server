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
	private:
		ON_PIPE_EVENT on_pipe_event;
		PRIVATE_READONLY_DEFAULT(uint32_t, max_events_num);
		PRIVATE_RW_DEFAULT(int, epoll_wait_time_out);//epoll_wait函数调用时超时时间间隔
	public:
		lib_tcp_server_epoll_t(uint32_t max_events_num);
		virtual ~lib_tcp_server_epoll_t(){
			destroy();
		}
		virtual int register_on_functions(const on_functions_tcp_server* functions){
			return 0;
		}
		virtual int create();
		virtual int listen(uint32_t listen_num);
		virtual int run();
		virtual int stop(){
			return 0;
		}
		virtual int add_connect(int fd, E_FD_TYPE fd_type, struct sockaddr_in* peer);
		int add_events(int fd, uint32_t flag);
		void register_pipe_event_fn(ON_PIPE_EVENT fn);
	private:
		lib_tcp_server_epoll_t(const lib_tcp_server_epoll_t& cr);
		lib_tcp_server_epoll_t& operator=(const lib_tcp_server_epoll_t& cr);
		int destroy();
	};
}//end namespace ice