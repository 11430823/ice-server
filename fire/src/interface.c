#include "net_tcp.h"
#include "interface.h"

uint32_t fire::client_info_t::get_ip()
{
	return ice::lib_tcp_client_t::get_ip();
}

char* fire::client_info_t::get_ip_str()
{
	return ice::lib_tcp_client_t::get_ip_str();
}

int fire::client_info_t::send( const void* data, uint32_t len )
{
	int send_len = lib_tcp_t::send(data, len);
	if (-1 == send_len){
		g_net_server.get_server_epoll()->get_on_functions()->on_cli_conn_closed(this->get_fd());
		this->close();
	} else if (send_len < (int)len){
		this->send_buf.pop_front(send_len);
		g_net_server.get_server_epoll()->mod_events(this->fd, EPOLLIN | EPOLLOUT);
	} else {
		this->send_buf.pop_front(send_len);
	}
	return send_len;
}

