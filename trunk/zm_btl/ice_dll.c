#include "../include/dll.h"
#include <stdint.h>
#include <stdio.h>

#include "log.h"
#include "timer.h"
#include "proto.h"
#include "dispatch.h"
#include "data.h"

/**
  * @brief Initialize service
  *
  */
extern "C" int init_service(int isparent)
{
	if(isparent){
	}else{
		DEBUG_LOG("======server start======");
		setup_timer();
		g_cmd.init_handle();
	}
	return 0;
}

/**
  * @brief Finalize service
  *
  */
extern "C" int fini_service(int isparent)
{
	if (!isparent) {
		destroy_timer();
		DEBUG_LOG("======server done======");
	}

	return 0;
}

/**
  * @brief Process events such as timers and signals
  *
  */
extern "C" void on_events()
{
	handle_timer();
}

/**
  * @brief Return length of the receiving package
  *
  */
extern "C" int get_pkg_len(int fd, const void* avail_data, int avail_len, int isparent)
{
	if (avail_len < 4) {
		return 0;
	}

	uint32_t len = *(uint32_t *)avail_data;
 	if (isparent) {
 		if (len > 32*1024 || len < (int)sizeof(cli_proto_head_t)) {
 			return -1;
 		}
 	}
	INFO_LOG("[fd:%d, avail_len:%d, isparent:%d, avail_data:%s]", fd, avail_len, isparent, (char*)avail_data);
	return len;
}

/**
  * @brief Process packages from clients
  *
  */
extern "C" int proc_pkg_from_client(void* data, int len, fdsession_t* fdsess)
{
	/* 返回非零，断开FD的连接 */ 
	INFO_LOG("[fd:%d, len:%d, data:%s]", fdsess->fd, len, (char*)data);
	cli_proto_head_t* p = (cli_proto_head_t*)data;
	INFO_LOG("[cmd:%d, id:%d, len:%d, ret:%d, seq_num:%d]", p->cmd, p->id, p->len, p->ret, p->seq_num);
	return dispatch_client(data, fdsess);
}

/**
  * @brief Process packages from servers
  *
  */
extern "C" void proc_pkg_from_serv(int fd, void* data, int len)
{
}

/**
  * @brief Called each time on client connection closed
  *
  */
extern "C" void on_client_conn_closed(int fd)
{
}

/**
  * @brief Called each time on close of the fds created by the child process
  *
  */
extern "C" void on_fd_closed(int fd)
{
}

/**
  * @brief Called to process mcast package from the address and port configured in the config file
  */
extern "C" void proc_mcast_pkg(void* data, int len)
{
}

/* *
 * @brief Called to reload global data(mainly global function address.etc)
 */
extern "C" int reload_global_data()
{
	return 0;
}

extern "C" int	proc_udp_pkg(int fd, const void* avail_data, int avail_len ,struct sockaddr_in * from, socklen_t fromlen )
{
	return 0;
}

extern "C" void sync_service_info(uint32_t svr_id, const char* svr_name, const char* svr_ip, in_port_t port, int flag)
{
}



