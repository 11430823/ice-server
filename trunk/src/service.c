#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "service.h"
#include "bind_conf.h"
#include "net.h"
#include "daemon.h"
#include "dll.h"
#include "mcast.h"
#include "net.h"
#include "net_if.h"
#include "log.h"
#include "bench_conf.h"

service_t g_service;
int					is_parent = 1;
config_cache_t		config_cache;
fd_array_session_t	fds;

static inline void
free_fdsess(void* fdsess)
{
	g_slice_free1(sizeof(fdsession_t), fdsess);
}
static inline int
handle_init(bind_config_elem_t* bc_elem)
{
	config_cache.idle_timeout = 10;
	config_cache.bc_elem      = bc_elem;

	fds.cn = g_hash_table_new_full(g_int_hash, g_int_equal, 0, free_fdsess);

	// create multicast socket if function `proc_mcast_pkg` is defined
	if (g_dll.proc_mcast_pkg && (create_mcast_socket() == -1)) {
		// return -1 if fail to create mcast socket
		return -1;
	}

	//  [9/12/2011 meng]
#if 0
	if (config_get_strval("addr_mcast_ip")) {
		if (create_addr_mcast_socket() == 0) {
			send_addr_mcast_pkg(addr_mcast_1st_pkg);
		} else {
			// return -1 if fail to create mcast socket
			return -1;
		}
	}
#endif
	return (g_dll.init_service ? g_dll.init_service(0) : 0);	
}

static inline int
handle_fini()
{
	int i;
	for (i = 0; i <= epi.maxfd; ++i) {
		if ( (epi.fds[i].type == fd_type_remote) && (epi.fds[i].cb.sendlen > 0) ) {
			return 0;
		}
	}

	if ( g_dll.fini_service && (g_dll.fini_service(0) != 0) ) {
		return 0;
	}

	g_hash_table_destroy(fds.cn);
	return 1;
}

static inline void
handle_process(uint8_t* recvbuf, int rcvlen, int fd)
{

	fdsession_t* fdsess = get_fdsess(fd);
	if (fdsess) {

		if (g_dll.proc_pkg_from_client(recvbuf, rcvlen, fdsess)) {

			close_client_conn(fd);
		}
	}
}
static inline void
add_fdsess(fdsession_t* fdsess)
{
	g_hash_table_insert(fds.cn, &(fdsess->fd), fdsess);
	++(fds.count);
}
static inline int
handle_open(const shm_block_t* mb)
{
	fdsession_t* fdsess = get_fdsess(mb->fd);
	if (fdsess || (mb->length != (sizeof(shm_block_t) + sizeof(skinfo_t)))) {
//		ERROR_LOG("handle_open OPEN_BLOCK, fd=%d length=%d", mb->fd, mb->length);
		return -1;
	} else {
		fdsess               = (fdsession_t*)g_slice_alloc(sizeof *fdsess);
		fdsess->fd           = mb->fd;
		fdsess->id           = mb->id;
		fdsess->remote_port  = *(uint16_t*)mb->data;
		fdsess->remote_ip    = *(uint32_t*)&mb->data[2];
		add_fdsess(fdsess);
	}

	return 0;
}
void handle_recv_queue()
{
	struct shm_queue_t* recvq = &(config_cache.bc_elem->recvq);
	struct shm_queue_t* sendq = &(config_cache.bc_elem->sendq);

	struct shm_block_t* mb;
	while (shmq_pop((shm_queue_t*)recvq, &mb) == 0) {
		switch (mb->type) {
		case DATA_BLOCK:
			if ( mb->length > sizeof(*mb) ) {
				handle_process(mb->data, mb->length - sizeof(*mb), mb->fd);
			}
			break;
		case OPEN_BLOCK:
			if ( handle_open(mb) == -1 ) {
				mb->type    = FIN_BLOCK;
				mb->length  = sizeof(*mb);
				shmq_push((shm_queue_t*)sendq, mb, NULL);
			}
			break;
		case CLOSE_BLOCK:
			handle_close(mb->fd);
			break;		
		default:
			break;
		}
	}
}

fdsession_t* get_fdsess( int fd )
{
	return (fdsession_t*)g_hash_table_lookup(fds.cn, &fd);
}
static inline void
remove_fdsess(int fd)
{
	g_hash_table_remove(fds.cn, &fd);
	--(fds.count);
}
int handle_close(int fd)
{
	fdsession_t* fdsess = get_fdsess(fd);
	if (!fdsess) {
//		ERROR_RETURN( ("connection %d had already been closed", fd), -1 );
		return -1;
	}

	assert(fds.count > 0);

	g_dll.on_client_conn_closed(fd);

	remove_fdsess(fd);
	return 0;
}

void service_t::worker_process( struct bind_config_t* bc, int bc_elem_idx, int n_inited_bc )
{
	bind_config_elem_t* bc_elem = bc->get_elem(bc_elem_idx);

	is_parent = 0;

	// release resources inherited from parent process
	g_shmq.close_pipe(bc, n_inited_bc, 1);
	shmq_destroy(bc_elem, n_inited_bc);
	net_exit();

	g_net.init(g_bench_conf.get_max_fd_num(), 2000);
	do_add_conn(bc_elem->recvq.pipe_handles[0], fd_type_pipe, 0, 0);

	if ( handle_init(bc_elem) != 0 ) {
		ERROR_LOG("fail to init worker process. olid=%u olname=%s", bc_elem->id, bc_elem->name.c_str());
		goto fail;
	}

	while ( !g_daemon.stop || !handle_fini() ) {
		net_loop(100, page_size, 0);
	}
fail:
	do_destroy_shmq(bc_elem);
	net_exit();
	g_dll.unregister_data_plugin();
	g_dll.unregister_plugin();
	exit(0);
}
