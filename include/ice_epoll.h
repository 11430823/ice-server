/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		
*********************************************************************/
#pragma  once

#include <sys/epoll.h>

#include <ice_lib/list.h>

#pragma pack(1)

struct conn_buf_t {
	uint32_t	rcvprotlen;
	uint32_t	recvlen;
	uint32_t	sendlen;
	uint32_t	sndbufsz;
	uint8_t*	recvptr;
	uint8_t*	sendptr;
};

struct skinfo_t {
	uint16_t	remote_port;
	uint32_t	remote_ip;
	time_t	last_tm;
};

struct fdinfo_t {
	uint32_t	id;
	int			sockfd;
	uint8_t		type;
	uint8_t		flag;
	conn_buf_t	cb;
	skinfo_t	sk;
	struct bind_config_elem_t*	bc_elem;
	// callback for asynchronous connect
	void		(*callback)(int fd, void* arg);
	void*		arg;

	list_head_t	list;
};

struct epinfo_t {
	fdinfo_t*	fds;
	struct epoll_event*	evs;
	list_head_t	close_head;
	list_head_t	etin_head;
	int			epfd;
	int			maxfd;
	int			max_ev_num;
	int			count;
};

#pragma pack()

extern epinfo_t g_epi;