#include <errno.h>
#include <string.h>
#include <netinet/in.h>

#include <ice_lib/lib_util.h>
#include <ice_lib/log.h>

#include "ice_epoll.h"
ep_info_t g_epi;

namespace {
	int add_events (int epfd, int fd, uint32_t flag)
	{
		struct epoll_event ev;

		ev.events = flag;
		ev.data.fd = fd;
epoll_add_again:
		if (unlikely (0 != epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev))) {
			if (errno == EINTR){
				goto epoll_add_again;
			}else{
				ERROR_LOG ("epoll_ctl add %d error: %m, %d", fd, -1);
				return -1;
			}
		}
		return 0; 
	}
}//end namespace

int ep_info_t::do_add_conn(int fd, uint8_t type, struct sockaddr_in *peer,
							struct bind_config_elem_t* bc_elem)
{
	static uint32_t seq = 0;
	uint32_t flag;

	switch (type) {
	case fd_type_pipe:
	case fd_type_mcast:
	case fd_type_addr_mcast:
	case fd_type_udp:
		flag = EPOLLIN;
		break;
	case fd_type_asyn_connect:
		flag = EPOLLOUT;
		break;
	default:
		flag = EPOLLIN | EPOLLET;
		break;
	}

	if (0 != add_events(m_fd, fd, flag)) {
		return -1;
	}

	memset(&m_fds[fd], 0, sizeof(struct fdinfo_t));
	m_fds[fd].sockfd = fd;
	m_fds[fd].type = type;

	m_fds[fd].id = ++seq;
	if (0 == seq) {
		m_fds[fd].id = ++seq;
	}
	if (peer) {
		m_fds[fd].sk.remote_ip = peer->sin_addr.s_addr;
		m_fds[fd].sk.remote_port = peer->sin_port;
		//mark 没有初始化SK中的时间参数?
	}
	m_fds[fd].bc_elem = bc_elem;
	m_max_fd = m_max_fd > fd ? m_max_fd : fd;
	m_fd_count++;

	TRACE_LOG("add fd:%d, type:%d, id:%u", fd, type, m_fds[fd].id);
	return 0;
}