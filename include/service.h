#pragma once

#include <stdint.h>
#include <netinet/in.h>

#include <glib/ghash.h>

typedef struct fd_array_session {
	int			count;
	GHashTable*	cn;
} fd_array_session_t;
struct config_cache_t{
	int					idle_timeout;
	struct bind_config_elem_t*	bc_elem;
};
struct fdsession_t {
	int			fd;
	uint32_t	id;
	in_port_t	remote_port;
	uint32_t	remote_ip;
};

fdsession_t* get_fdsess(int fd);

extern bool	g_is_parent;
extern fd_array_session_t	fds;
void run_worker_process(struct bind_config_t* bc, int bc_elem_idx, int n_inited_bc);
void handle_recv_queue();
int handle_close(int fd);

class service_t
{
public:
	void worker_process(int bc_elem_idx, int n_inited_bc);
	struct bind_config_elem_t*	m_bind_elem;
	static const uint32_t idle_timeout = 10;
	uint32_t get_id();
	const char* get_name();
	const char* get_ip();
	in_port_t get_port();
protected:
private:
};

extern service_t g_service;