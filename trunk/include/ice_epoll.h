/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		
*********************************************************************/
#pragma  once

#include <sys/epoll.h>

#include <lib_list.h>

#define CN_NEED_CLOSE	0x01
#define CN_NEED_POLLIN	0x02//普通或优先级带数据可读

extern uint32_t SEND_BUF_LIMIT_SIZE;
extern int32_t EPOLL_TIME_OUT;

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
	time_t		last_tm;
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
	fdinfo_t*	m_fds;//连接的FD信息
	epoll_event*	m_evs;
	list_head_t	m_close_head;
	list_head_t	m_etin_head;
	int			m_fd;
	int			m_max_fd;//所有FD里最大的FD的值.
	int			m_max_ev_num;//is the maximum number of events to be returned
	int			m_fd_count;//FD的数量
	//************************************
	// Brief:     
	// Returns:   int 0:success,-1:error
	// Parameter: int size
	// Parameter: int maxevents
	//************************************
	int init(int size, int maxevents);
	int loop(int max_len);
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
	fd_type_listen = 1,
	fd_type_pipe = 2,
	fd_type_remote = 3,
	fd_type_mcast = 4,
	fd_type_addr_mcast = 5,
	fd_type_udp = 6,
	fd_type_asyn_connect = 7,
};

int net_start(const char* listen_ip, in_port_t listen_port, struct bind_config_elem_t* bc_elem);
void net_exit ();

inline void del_from_etin_queue (int fd);
void do_del_conn(int fd, bool is_conn);
int do_write_conn(int fd);
int mod_events(int epfd, int fd, uint32_t flag);
inline void add_to_etin_queue (int fd);


extern ep_info_t g_epi;