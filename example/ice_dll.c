#include "../include/dll.h"
#include <stdint.h>


#include <stdio.h>
/**
  * @brief Initialize service
  *
  */
extern "C" int init_service(int isparent)
{
	if(isparent){
		printf("int init_service(1)\r\n");
	}else{
		printf("int init_service(0)\r\n");
	}
	return 0;
}

/**
  * @brief Finalize service
  *
  */
extern "C" int fini_service(int isparent)
{
//	if (!isparent) {
//		destroy_timer();
//	}
	return 0;
}

/**
  * @brief Process events such as timers and signals
  *
  */
extern "C" void on_events()
{
//	handle_timer();
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
// 	if (isparent) {
// 		if (len > e_msg_len_max || len < (int)sizeof(btl_proto_head_t)) {
// 			return -1;
// 		}
// 	}
	return len;
}

/**
  * @brief Process packages from clients
  *
  */
extern "C" int proc_pkg_from_client(void* data, int len, fdsession_t* fdsess)
{
	/* 返回非零，断开FD的连接 */ 
	return 0;
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



