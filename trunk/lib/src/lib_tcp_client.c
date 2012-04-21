#include "lib_include.h"
#include "lib_tcp_client.h"

char* ice::lib_tcp_client_info_t::get_ip_str()
{
	struct in_addr a;
	a.s_addr = this->ip;
	return inet_ntoa(a);
}

ice::lib_tcp_client_info_t::lib_tcp_client_info_t()
{
	this->init();
}

void ice::lib_tcp_client_info_t::close()
{
	lib_file_t::close_fd(this->fd);
	this->init();
}

void ice::lib_tcp_client_info_t::init()
{
	this->fd = -1;
	this->fd_type = FD_TYPE_UNUSED;
	this->port = 0;
	this->ip = 0;
	this->last_tm = 0;
	this->recv_buf.clean();
	this->send_buf.clean();
}

ice::lib_tcp_client_info_t::~lib_tcp_client_info_t()
{

}

uint16_t ice::lib_tcp_client_info_t::get_port()
{
	return this->port;
}

uint32_t ice::lib_tcp_client_info_t::get_ip()
{
	return this->ip;
}
