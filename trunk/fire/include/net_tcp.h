/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		网络服务接口
*********************************************************************/
#pragma  once

#include <map>
#include <string>
#include <sys/epoll.h>

#include <lib_net/lib_tcp_server.h>
#include <lib_util.h>

#include "mcast.h"

struct on_functions_tcp_server_epoll : public ice::on_functions_tcp_srv 
{
};

class tcp_server_epoll_t : public ice::lib_tcp_srv_t
{
public:
	typedef int (*ON_PIPE_EVENT)(int fd, epoll_event& r_evs);
	PRIVATE_RW_DEFAULT(ON_PIPE_EVENT, on_pipe_event);
	PRIVATE_RW_DEFAULT(int, epoll_wait_time_out);//epoll_wait函数调用时超时时间间隔
	PRIVATE_R_DEFAULT(on_functions_tcp_server_epoll*, on_functions);//回调函数
public:
	tcp_server_epoll_t(uint32_t max_events_num);
	virtual ~tcp_server_epoll_t();
	virtual void register_on_functions(const ice::on_functions_tcp_srv* functions);
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
	virtual ice::lib_tcp_peer_info_t* add_connect(int fd, 
		ice::E_FD_TYPE fd_type, const char* ip, uint16_t port);
public:
	int mod_events(int fd, uint32_t flag);
	int add_events(int fd, uint32_t flag);
	/**
	 * @brief	关闭连接
	 * @param	bool do_calback true:调用客户端注册的回调函数.通知被关闭, false:不做通知
	 */
	void close_peer(ice::lib_tcp_peer_info_t& fd_info, bool do_calback = true);
private:
	tcp_server_epoll_t(const tcp_server_epoll_t& cr);
	tcp_server_epoll_t& operator=(const tcp_server_epoll_t& cr);

	void handle_peer_msg(ice::lib_tcp_peer_info_t& fd_info);
	void handle_peer_mcast_msg(ice::lib_tcp_peer_info_t& fd_info);
	void handle_peer_add_mcast_msg(ice::lib_tcp_peer_info_t& fd_info);
	void handle_listen();
	int handle_send(ice::lib_tcp_peer_info_t& fd_info);
};


class net_server_t
{
	PRIVATE_R_DEFAULT(tcp_server_epoll_t*, server_epoll);
public:
	net_server_t(void);
	virtual ~net_server_t(){}
	//************************************
	// Brief:     
	// Returns:   int 0:success,-1:error
	//************************************
	int create(uint32_t max_fd_num);
	int destroy();
protected:
private:
	net_server_t(const net_server_t &cr);
	net_server_t & operator=( const net_server_t &cr);
};



extern net_server_t g_net_server;
