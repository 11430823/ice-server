#include <stdint.h>
#include <stdio.h>

#include <lib_log.h>
#include <bench_conf.h>
#include <interface.h>

#pragma pack(1)
/* SERVER和CLIENT的协议包头格式 */
struct cli_proto_head_t {
	uint32_t len; /* 协议的长度 */
	uint32_t cmd; /* 协议的命令号 */
	uint32_t id; /* 账号 */
	uint32_t seq_num;/* 序列号 */
	uint32_t ret; /* S->C, 错误码 */
	uint8_t body[]; /* 包体信息 */
};
#pragma pack()

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
		ice::lib_tcp_server_info_t ser;
		ser.connect("192.168.0.102", 8001, 0, false);
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
extern "C" int on_get_pkg_len(ice::lib_tcp_client_info_t* cli_fd_info, const void* data, uint32_t len)
{
	if (len < 4){
		return 0;
	}

	uint32_t pkg_len = 0;
	pkg_len = *(uint32_t*)(data);
	TRACE_LOG("[fd:%d, len:%d, pkg_len:%u]", cli_fd_info->get_fd(), len, pkg_len);
	if (pkg_len < sizeof(cli_proto_head_t) || pkg_len > g_bench_conf.get_page_size_max()){
		ERROR_LOG("head len err [len=%u]", pkg_len);
		return -1;
	}

	return pkg_len;
}

/**
  * @brief Process packages from clients
  *
  */
extern "C" int on_cli_pkg(const void* pkg, int pkglen, ice::lib_tcp_client_info_t* cli_fd_info)
{
	/* 返回非零，断开FD的连接 */ 
	cli_proto_head_t* head = (cli_proto_head_t*)pkg;
	TRACE_LOG("[len:%u, cmd:%u, seq:%u, ret:%u, uid:%u, fd:%d, pkglen:%d]",
		head->len, head->cmd, head->seq_num, head->ret, head->id, cli_fd_info->get_fd(), pkglen);
	fire::send(cli_fd_info, pkg, pkglen);
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
	TRACE_LOG("[fd%d]", fd);
}

/**
  * @brief Called each time on close of the fds created by the child process
  *
  */
extern "C" void on_svr_conn_closed(int fd)
{
}



