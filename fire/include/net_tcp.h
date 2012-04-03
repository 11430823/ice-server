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

class net_server_t
{
public:
	struct bind_config_elem_t* bc_elem;
	//************************************
	// Brief:     
	// Returns:   int 0:success,-1:error
	//************************************
	int create(uint32_t max_fd_num);
	int listen(const char* listen_ip, in_port_t listen_port, struct bind_config_elem_t* bc_elem);
	int daemon_run();

	int destroy();

	PRIVATE_READONLY_DEFAULT(lib_tcp_server_epoll_t*, server_epoll);
public:
	net_server_t(void){
		this->bc_elem = NULL;
		this->server_epoll = NULL;
	}
	virtual ~net_server_t(){
		
	}
protected:
	
private:
	net_server_t(const net_server_t &cr);
	net_server_t & operator=( const net_server_t &cr);
};

int mod_events(int epfd, int fd, uint32_t flag);

extern net_server_t g_net_server;
