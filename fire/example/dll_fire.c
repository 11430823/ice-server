#include <stdint.h>
#include <stdio.h>

#include <lib_log.h>
#include <lib_tcp_server.h>

/**
  * @brief Initialize service
  *
  */
extern "C" int on_init(int isparent)
{
	if(isparent){
		DEBUG_LOG("======daemon start======");
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
	if (isparent) {
		DEBUG_LOG("======daemon done======");
	}else{
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
extern "C" int on_get_pkg_len(int fd, const void* data, int len, int isparent)
{
	if (isparent){
		INFO_LOG("parent [fd:%d, len:%d, data:%s]", fd, len, (char*)data);
		if (len < 4) {
			return 0;
		}
	}else{
		INFO_LOG("child [fd:%d, len:%d, data:%s]", fd, len, (char*)data);
		if (len < 4) {
			return 0;
		}
	}
	return len;
}

/**
  * @brief Process packages from clients
  *
  */
extern "C" int on_cli_pkg(void* pkg, int pkglen, ice::cli_fd_info_t* cli_fd_info)
{
	/* 返回非零，断开FD的连接 */ 
	INFO_LOG("[fd:%d, len:%d, data:%s]", cli_fd_info->fd, pkglen, (char*)pkg);

	return 0;
}

/**
  * @brief Process packages from servers
  *
  */
extern "C" void on_srv_pkg(int fd, void* pkg, int pkglen)
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
extern "C" void on_svr_conn_closed(int fd)
{
}



