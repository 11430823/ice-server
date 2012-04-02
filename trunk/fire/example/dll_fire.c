#include <stdint.h>
#include <stdio.h>

#include <ice_dll.h>
#include <lib_log.h>

/**
  * @brief Initialize service
  *
  */
extern "C" int on_init(int isparent)
{
	if(isparent){
	}else{
		DEBUG_LOG("======server start======");
	}
	return 0;
}

/**
  * @brief Finalize service
  *
  */
extern "C" int on_fini(int isparent)
{
	if (!isparent) {
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
}

/**
  * @brief Return length of the receiving package
  *
  */
extern "C" int on_get_pkg_len(int fd, const void* avail_data, int avail_len, int isparent)
{
	if (avail_len < 4) {
		return 0;
	}
	uint32_t len = 0;
	INFO_LOG("[fd:%d, avail_len:%d, isparent:%d, avail_data:%s]", fd, avail_len, isparent, (char*)avail_data);
	return len;
}

/**
  * @brief Process packages from clients
  *
  */
extern "C" int on_cli_pkg(void* data, int len, fdsession_t* fdsess)
{
	/* 返回非零，断开FD的连接 */ 
	INFO_LOG("[fd:%d, len:%d, data:%s]", fdsess->fd, len, (char*)data);

	return 0;
}

/**
  * @brief Process packages from servers
  *
  */
extern "C" void on_srv_pkg(int fd, void* data, int len)
{
}

/**
  * @brief Called each time on client connection closed
  *
  */
extern "C" void on_cli_conn_closed(int fd)
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
extern "C" void on_mcast_pkg(void* data, int len)
{
}

extern "C" int	on_udp_pkg(int fd, const void* avail_data, int avail_len ,struct sockaddr_in * from, socklen_t fromlen )
{
	return 0;
}

extern "C" void on_sync_srv_info(uint32_t svr_id, const char* svr_name, const char* svr_ip, in_port_t port, int flag)
{
}



