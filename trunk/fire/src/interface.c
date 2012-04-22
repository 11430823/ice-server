#include <string.h>
#include <lib_tcp_client.h>
#include <lib_tcp.h>
#include <lib_tcp_server_epoll.h>
#include <lib_log.h>

#include "net_tcp.h"
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
		g_net_server.get_server_epoll()->mod_events(peer_info->get_fd(), EPOLLIN | EPOLLOUT);
	}
	return send_len;
}

int fire::connect( ice::lib_tcp_peer_info_t* svr_info )
{
	int fd = svr_info->connect(1, false);
	if (-1 == fd){
	}else{
		sockaddr_in sa_in;
		memset(&sa_in, 0, sizeof(sa_in));
		sa_in.sin_addr.s_addr = inet_addr(svr_info->get_ip_str());
		sa_in.sin_family = PF_INET;
		sa_in.sin_port = htons(svr_info->get_port());
		TRACE_LOG("%u, %u", sa_in.sin_port, svr_info->get_port());
		g_net_server.get_server_epoll()->add_connect(fd, ice::FD_TYPE_SVR, svr_info->get_ip_str(), svr_info->get_port());
	}

	return fd;
}
