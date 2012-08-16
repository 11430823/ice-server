#include <string.h>

#include <lib_net/lib_tcp_client.h>
#include <lib_net/lib_tcp.h>
#include <lib_log.h>

#include "net_tcp.h"
#include "service.h"
#include "daemon.h"
#include "interface.h"

int fire::s2peer( ice::lib_tcp_peer_info_t* peer_info, const void* data, uint32_t len )
{
	int send_len = peer_info->send(data, len);
	if (-1 == send_len){
		WARN_LOG("send err [len:%u, send_len:%d]", len, send_len);
		g_net_server.get_server_epoll()->close_peer(*peer_info);
	} else if (send_len < (int)len){
		WARN_LOG("send too long [len:%u, send_len:%d]", len, send_len);
		peer_info->send_buf.push_back((char*)data + send_len, len - send_len);
		g_net_server.get_server_epoll()->mod_events(peer_info->get_fd(), EPOLLIN | EPOLLOUT | EPOLLRDHUP);//todo ? EPOLLRDHUP
	}
	return send_len;
}

ice::lib_tcp_peer_info_t* fire::connect( const std::string& ip, uint16_t port )
{
	int fd = ice::lib_tcp_t::connect(ip, port, 1, false);
	if (-1 == fd){
		ERROR_LOG("[ip:%s, port:%u]", ip.c_str(), port);
	}else{
		TRACE_LOG("[ip:%s, port:%u]", ip.c_str(), port);
		return g_net_server.get_server_epoll()->add_connect(fd, ice::FD_TYPE_SVR, ip.c_str(), port);
	}

	return NULL;
}

ice::lib_tcp_peer_info_t* fire::connect( const char* svr_name )
{
	std::string ip;
	uint16_t port = 0;
	if (!g_addr_mcast.get_1st_svr_ip_port(svr_name, ip, port)){
		ERROR_LOG("[svr_name:%s]", svr_name);
		return NULL;
	}
	
	return fire::connect(ip, port);
}


uint32_t fire::get_server_id()
{
	return g_service.get_bind_elem_id();
}

const char* fire::get_server_name()
{
	return g_service.get_bind_elem_name();
}

bool fire::is_parent()
{
	return g_is_parent;
}
