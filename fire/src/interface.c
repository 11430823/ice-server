#include "interface.h"

int fire::send( int fd, const void* data, uint32_t len )
{
	int send_len = g_net_server.get_server_epoll()->cli_fd_infos[fd].send(data, len);
	if (send_len < 0){
		g_net_server.get_server_epoll()->on_functions->on_cli_conn_closed(fd);
		g_net_server.get_server_epoll()->cli_fd_infos[fd]->close();
	}
	return send_len;
}

