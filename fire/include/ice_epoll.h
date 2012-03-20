/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		
*********************************************************************/
#pragma  once

#include <sys/epoll.h>
#include <netinet/in.h>

#include <lib_list.h>

#pragma pack(1)
struct fdinfo_t {
	uint32_t	id;
	int			fd;
	uint8_t		fd_type;
	uint16_t	remote_port;
	uint32_t	remote_ip;
	uint32_t	last_tm;
};
#pragma pack()

class ep_info_t
{
public:
	fdinfo_t*	m_fds;//连接的FD信息
	int			m_max_ev_num;//is the maximum number of events to be returned
	struct bind_config_elem_t* bc_elem;
	//************************************
	// Brief:     
	// Returns:   int 0:success,-1:error
	// Parameter: int maxevents
	//************************************
	int init(int maxevents);
	int start(const char* listen_ip, in_port_t listen_port, struct bind_config_elem_t* bc_elem);

	int exit();
	int loop(int max_len);
	int child_loop(int max_len);
	int do_add_conn(int fd, uint8_t type, struct sockaddr_in *peer);
	PROPERTY_READONLY_DEFAULT(int, fd);
	PROPERTY_READONLY_DEFAULT(int, max_fd);//所有FD里最大的FD的值.
	PROPERTY_READONLY_DEFAULT(int, max_ev_num);//is the maximum number of events to be returned
public:
	ep_info_t(void){
		this->EPOLL_TIME_OUT = -1;
		this->bc_elem = NULL;
		this->m_fds = NULL;
		this->m_max_ev_num = 0;
		this->fd = 0;
		this->max_fd = 0;
		this->max_ev_num = 0;
	}
	//virtual ~ep_info_t(){}
	int32_t EPOLL_TIME_OUT;
protected:
	
private:
	ep_info_t(const ep_info_t &cr);
	ep_info_t & operator=( const ep_info_t &cr);
	int add_events(int fd, uint32_t flag);
};

enum E_FD_TYPE{
	fd_type_unused = 0,
	fd_type_listen = 1,
	fd_type_pipe = 2,
	fd_type_remote = 3,
	fd_type_mcast = 4,
	fd_type_addr_mcast = 5,
	fd_type_udp = 6,
	fd_type_asyn_connect = 7,
};

int mod_events(int epfd, int fd, uint32_t flag);

extern ep_info_t g_epi;
