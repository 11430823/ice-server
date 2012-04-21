#include <lib_tcp_client.h>
#include <lib_tcp.h>
#include <lib_tcp_server_epoll.h>

#include "net_tcp.h"
#include <lib_log.h>
#include "interface.h"

int fire::send( ice::lib_tcp_client_info_t* cli_info, const void* data, uint32_t len )
{
	int send_len = cli_info->send(data, len);
	if (-1 == send_len){
		WARN_LOG("send err [len:%u, send_len:%d]", len, send_len);
		g_net_server.get_server_epoll()->close_peer(*cli_info);
	} else if (send_len < (int)len){
		WARN_LOG("send too long [len:%u, send_len:%d]", len, send_len);
		cli_info->send_buf.push_back((char*)data + send_len, len - send_len);
		g_net_server.get_server_epoll()->mod_events(cli_info->get_fd(), EPOLLIN | EPOLLOUT);
	}
	return send_len;
}
