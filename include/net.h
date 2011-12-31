#pragma  once

#include <string>

#include <ice_lib/lib_list.h>

#define CN_NEED_CLOSE	0x01
#define CN_NEED_POLLIN	0x02


extern const uint32_t PAGE_SIZE;
extern uint32_t g_send_buf_limit_size;
extern int32_t EPOLL_TIME_OUT;

int net_start(const char* listen_ip, in_port_t listen_port, struct bind_config_elem_t* bc_elem);
void net_exit ();
int net_loop(int max_len);
inline void del_from_etin_queue (int fd);
void do_del_conn(int fd, bool is_conn);
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