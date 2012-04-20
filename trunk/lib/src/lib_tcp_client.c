#include <sys/epoll.h>

#include "lib_tcp_client.h"

char* ice::lib_tcp_client_t::get_ip()
{
	struct in_addr a;
	a.s_addr = this->remote_ip;
	return inet_ntoa(a);
}

ice::lib_tcp_client_t::lib_tcp_client_t()
{
	this->init();
}

void ice::lib_tcp_client_t::close()
{
	lib_file_t::close_fd(this->fd);
	this->init();
}

void ice::lib_tcp_client_t::init()
{
	this->fd = -1;
	this->fd_type = FD_TYPE_UNUSED;
	this->remote_port = 0;
	this->remote_ip = 0;
	this->last_tm = 0;
	this->recv_buf.clean();
}

int ice::lib_tcp_client_t::send( const void* buf, int total )
{
	int send_len = lib_tcp_t::send(buf, total);
	if (send_len < total){
		this->send_buf.pop_front(send_len);
		g_net_server.get_server_epoll()->mod_events(this->fd, EPOLLIN | EPOLLOUT);
	} else {
		this->send_buf.pop_front(send_len);
	}
	return send_len;
}

ice::lib_tcp_client_t::~lib_tcp_client_t()
{

}
