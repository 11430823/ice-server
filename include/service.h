#pragma once
#include <stdint.h>
#include <glib/ghash.h>
typedef struct fd_array_session {
	int			count;
	GHashTable*	cn;
} fd_array_session_t;
struct config_cache_t{
	int					idle_timeout;
	struct bind_config_elem_t*	bc_elem;
};
typedef struct fdsession {
	int			fd;
	uint32_t	id;
	uint16_t	remote_port;
	uint32_t	remote_ip;
} fdsession_t;

fdsession_t* get_fdsess(int fd);

extern int					is_parent;
extern config_cache_t		config_cache;
extern fd_array_session_t	fds;
void run_worker_process(struct bind_config_t* bc, int bc_elem_idx, int n_inited_bc);
void handle_recv_queue();
int handle_close(int fd);

class service_t
{
public:
	void worker_process(struct bind_config_t* bc, int bc_elem_idx, int n_inited_bc);
protected:
private:
};

extern service_t g_service;