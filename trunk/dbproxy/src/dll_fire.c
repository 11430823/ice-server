/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		基础服务接口
*********************************************************************/

#include <stdint.h>
#include <stdio.h>

#include <lib_log.h>
#include <lib_timer.h>
#include <lib_msgbuf.h>
#include <lib_err_code.h>

#include <bench_conf.h>
#include <interface.h>

#include "route.h"
#include "service.h"

service_mgr_t g_service_mgr;

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
		if (0 != g_rotue_t.parser()){
			return -1;
		}
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
extern "C" int on_get_pkg_len(ice::lib_tcp_peer_info_t* cli_fd_info, const void* data, uint32_t len)
{
	if (len < 4){
		return 0;
	}

	ice::lib_recv_data_cli_t in(data);
	uint32_t pkg_len = in.get_len();
	TRACE_LOG("[fd:%d, len:%d, pkg_len:%u]", cli_fd_info->get_fd(), len, pkg_len);
	if (pkg_len < sizeof(ice::proto_head_t) || pkg_len > g_bench_conf.get_page_size_max()){
		ERROR_LOG("head len err [len=%u]", pkg_len);
		return -1;
	}

	return pkg_len;
}

/**
  * @brief Process packages from clients
  *
  */
extern "C" int on_cli_pkg(const void* pkg, int pkglen, ice::lib_tcp_peer_info_t* peer_fd_info)
{
	/* 返回非零，断开FD的连接 */
	ice::lib_recv_data_cli_t in(pkg);
	ice::proto_head_t head;
	in>>head.len>>head.cmd>>head.id>>head.seq>>head.ret;
	
	TRACE_LOG("[len:%u, cmd:%u, seq:%u, ret:%u, uid:%u, fd:%d, pkglen:%d]",
		head.len, head.cmd, head.seq, head.ret, head.id, peer_fd_info->get_fd(), pkglen);
	uint32_t db_type = 0;
	DB_SER* dbser = g_rotue_t.find_dbser(head.cmd, db_type);
	if (NULL != dbser){
		//todo 判断USERID
		if (E_DB_TYPE_1 == db_type){
		} else if (E_DB_TYPE_100 == db_type){
		}
	} else {
		//todo cmd命令没有定义
		ERROR_LOG("cmd no define [cmd:%u, id:%u]", head.cmd, head.id);
		ice::proto_head_t err_out;
		err_out.cmd = head.cmd;
		err_out.id = head.id;
		err_out.len = sizeof(err_out);
		err_out.ret = ice::E_LIB_ERR_CODE_DBPROXY_NO_FIND_CMD;
		err_out.seq = head.seq;
		ice::lib_send_data_cli_t out;
		out.set_head(err_out);
		fire::s2peer(peer_fd_info, (void*)out.data(), out.len());
	}

	return 0;
}

/**
  * @brief Process packages from servers
  *
  */
extern "C" void on_srv_pkg(const void* pkg, int pkglen, ice::lib_tcp_peer_info_t* peer_fd_info)
{
	TRACE_LOG("[fd:%d, ip:%s, port:%u]", peer_fd_info->get_fd(), peer_fd_info->get_ip_str().c_str(), peer_fd_info->get_port());
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
	TRACE_LOG("[fd%d]", fd);
	g_service_mgr.remove(fd);
}

/**
 * @brief	Called to process multicast packages from the specified `mcast_ip` and `mcast_port`. Called once for each package.
 */
extern "C" void	on_mcast_pkg(const void* data, int len)
{
	TRACE_LOG("[len:%d]", len);
}

/**
 * @brief	Called to process multicast packages from the specified `add_mcast_ip` and `add_mcast_port`. Called once for each package.
 */
extern "C"  void on_addr_mcast_pkg(uint32_t id, const char* name, const char* ip, uint16_t port, int flag/*1:可用.0:不可用*/)
{
	TRACE_LOG("[id:%u, name:%s, ip:%s, port:%u, flag:%d]", id, name, ip, port, flag);
	std::string peer_name = name;
	std::string peer_ip = ip;
	if (1 == flag){
		FOREACH(g_rotue_t.cmd_map, it){
			DB_SER& dbser = it->second;
			FOREACH(dbser, it_dbser){
				db_info_t& dbinfo = it_dbser->second;
				if (dbinfo.name == peer_name && dbinfo.ip == peer_ip){
					if (NULL == g_service_mgr.get_service(peer_name)){
						ice::lib_tcp_peer_info_t* peer_info = fire::connect(name);
						if (NULL != peer_info){
							g_service_mgr.add(peer_name, peer_info);
						}
					}
				}
			}
		}
	}
}

extern "C" void on_udp_pkg(int fd, const void* data, int len ,struct sockaddr_in* from, socklen_t fromlen)
{
}

