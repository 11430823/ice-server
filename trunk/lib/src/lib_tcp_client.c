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
	//todo 没有发送完毕放入epoll 发送队列中
	return lib_tcp_t::send(buf, total);
	// 			uint32_t flag = EPOLLOUT;
	// 			int ret = HANDLE_EINTR(::epoll_ctl(this->fd, EPOLL_CTL_ADD, fd, &ev));
}

ice::lib_tcp_client_t::~lib_tcp_client_t()
{

}
