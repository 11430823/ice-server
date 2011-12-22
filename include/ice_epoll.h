/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		
*********************************************************************/
#pragma  once

#include <sys/epoll.h>

#include <ice_lib/lib_list.h>

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

#pragma pack()

class ep_info_t
{
public:
	fdinfo_t*	m_fds;
	epoll_event*	m_evs;
	list_head_t	m_close_head;
	list_head_t	m_etin_head;
	int			m_fd;
	int			m_max_fd;//所有FD里最大的FD的值.
	int			m_max_ev_num;//is the maximum number of events to be returned
	int			m_fd_count;//FD的数量
	int do_add_conn(int fd, uint8_t type, struct sockaddr_in *peer, struct bind_config_elem_t* bc_elem);

public:
	ep_info_t(void){}
	//virtual ~ep_info_t(){}
protected:
	
private:
	ep_info_t(const ep_info_t &cr);
	ep_info_t & operator=( const ep_info_t &cr);
};

enum E_FD_TYPE{
	fd_type_unused = 0,
	fd_type_listen,
	fd_type_pipe,
	fd_type_remote,
	fd_type_mcast,
	fd_type_addr_mcast,
	fd_type_udp,
	fd_type_asyn_connect
};


extern ep_info_t g_epi;