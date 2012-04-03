#include <sys/epoll.h>
#include <string.h>

#include "lib_log.h"
#include "lib_file.h"
#include "lib_time.h"
#include "lib_tcp_server_epoll.h"

int ice::lib_tcp_server_epoll_t::run()
{
	if (0 == this->max_events_num){
		return -1;
	}
	while (this->is_run){
	}

	return 0;
}

ice::lib_tcp_server_epoll_t::lib_tcp_server_epoll_t(uint32_t max_events_num)
{
	this->max_events_num = max_events_num;
	this->epoll_wait_time_out = -1;
}

int ice::lib_tcp_server_epoll_t::listen(uint32_t listen_num)
{
	return 0;
}

int ice::lib_tcp_server_epoll_t::create()
{
	if ((this->fd = epoll_create(this->max_events_num)) < 0) {
		ALERT_LOG("EPOLL_CREATE FAILED [ERROR:%s]", strerror (errno));
		return -1;
	}

	this->cli_fd_infos = (struct cli_fd_info_t*) new cli_fd_info_t[this->max_events_num];
	if (NULL == this->cli_fd_infos){
		lib_file_t::close_fd(this->fd);
		ALERT_LOG ("CALLOC CLI_FD_INFO_T FAILED [MAXEVENTS=%d]", this->max_events_num);
		return -1;
	}
	return 0;
}

int ice::lib_tcp_server_epoll_t::add_events( int fd, uint32_t flag )
{
	epoll_event ev;
	ev.events = flag;
	ev.data.fd = fd;

	int ret = HANDLE_EINTR(::epoll_ctl(this->fd, EPOLL_CTL_ADD, fd, &ev));
	if (0 != ret){
		ERROR_LOG ("epoll_ctl add fd:%d error:%s", fd, strerror(errno));
		return -1;
	}

	return 0; 
}

int ice::lib_tcp_server_epoll_t::add_connect( int fd, E_FD_TYPE fd_type, struct sockaddr_in* peer )
{
	uint32_t flag;

	switch (fd_type) {
	default:
		flag = EPOLLIN;
		break;
	}

	if (0 != this->add_events(fd, flag)) {
		return -1;
	}

	cli_fd_info_t& cfi = this->cli_fd_infos[fd];
	cfi.init();
	cfi.fd = fd;
	cfi.fd_type = fd_type;
	cfi.last_tm = ice::lib_time_t::get_now_second();
	if (NULL != peer) {
		cfi.remote_ip = peer->sin_addr.s_addr;
		cfi.remote_port = peer->sin_port;
	}

	KDEBUG_LOG(0, "time now:%ld, fd:%d, fd type:%d", ice::lib_time_t::get_now_second(), fd, fd_type);
	return 0;
}

int ice::lib_tcp_server_epoll_t::destroy()
{
	for (int i = 0; i < this->cli_fd_value_max; i++) {
		cli_fd_info_t& cfi = cli_fd_infos[i];
		if (FD_TYPE_UNUSED == cfi.fd_type){
			continue;
		}
		lib_file_t::close_fd(i);
	}

	safe_delete_arr(this->cli_fd_infos);

	lib_file_t::close_fd(this->fd);
	return 0;
}
