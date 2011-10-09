#pragma  once

#include <string>

#include "list.h"
#define CN_NEED_CLOSE	0x01
#define CN_NEED_POLLIN	0x02
enum {
	fd_type_unused = 0,
	fd_type_listen,
	fd_type_pipe,
	fd_type_remote,
	fd_type_mcast,
	fd_type_addr_mcast,
	fd_type_udp,
	fd_type_asyn_connect
};
struct skinfo_t {
	uint16_t	remote_port;
	uint32_t	remote_ip;
	time_t	last_tm;
} __attribute__((packed));

struct conn_buf_t {
	uint32_t	rcvprotlen;
	uint32_t	recvlen;
	uint32_t	sendlen;
	uint32_t	sndbufsz;
	uint8_t*	recvptr;
	uint8_t*	sendptr;
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
struct epinfo {
	fdinfo_t*	fds;
	struct epoll_event*	evs;
	list_head_t	close_head;
	list_head_t	etin_head;
	int			epfd;
	int			maxfd;
	int			max_ev_num;
	int			count;
};
extern int g_listen_port;
extern std::string  g_listen_ip;
extern epinfo epi;
extern uint32_t page_size;
extern uint32_t g_send_buf_limit_size;
int do_add_conn(int fd, uint8_t type, struct sockaddr_in *peer, struct bind_config_elem_t* bc_elem);
int net_start(const char* listen_ip, in_port_t listen_port, bind_config_elem_t* bc_elem);
void net_exit ();
int net_loop(int timeout, int max_len, int is_conn);
inline void del_from_etin_queue (int fd);
void do_del_conn(int fd, int is_conn);
int do_write_conn(int fd);
int mod_events(int epfd, int fd, uint32_t flag);

class net_t
{
public:
	//************************************
	// Brief:     
	// Returns:   int 0:success,-1:error
	// Parameter: int size
	// Parameter: int maxevents
	//************************************
	int init(int size, int maxevents);
protected:
private:
};

extern net_t g_net;