#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <ice_lib/lib_util.h>
#include <ice_lib/log.h>

#include "service.h"
#include "bind_conf.h"
#include "daemon.h"
#include "ice_dll.h"
#include "mcast.h"
#include "net_if.h"
#include "bench_conf.h"

service_t g_service;
bool g_is_parent = true;
fd_array_session_t	g_fds_session;
namespace {
	void add_fdsess(fdsession_t* fdsess)
	{
		g_hash_table_insert(g_fds_session.cn, &(fdsess->fd), fdsess);
		++(g_fds_session.count);
	}
	void remove_fdsess(int fd)
	{
		g_hash_table_remove(g_fds_session.cn, &fd);
		--(g_fds_session.count);
	}

	void free_fdsess(void* fdsess)
	{
		g_slice_free1(sizeof(fdsession_t), fdsess);
	}
	int handle_init(bind_config_elem_t* bc_elem)
	{
		g_fds_session.cn = g_hash_table_new_full(g_int_hash, g_int_equal, 0, free_fdsess);
		return g_dll.on_init(0);	
	}
	int handle_fini()
	{
		for (int i = 0; i <= g_epi.m_max_fd; ++i) {
			if ( (g_epi.m_fds[i].type == fd_type_remote) && (g_epi.m_fds[i].cb.sendlen > 0) ) {
				return -1;
			}
		}

		if (0 != g_dll.on_fini(g_is_parent)) {
			return -1;
		}

		g_hash_table_destroy(g_fds_session.cn);
		return 0;
	}
}

fdsession_t* get_fdsess( int fd )
{
	return (fdsession_t*)g_hash_table_lookup(g_fds_session.cn, &fd);
}

static inline void
handle_process(uint8_t* recvbuf, int rcvlen, int fd)
{

	fdsession_t* fdsess = get_fdsess(fd);
	if (fdsess) {

		if (g_dll.on_cli_pkg(recvbuf, rcvlen, fdsess)) {

			close_client_conn(fd);
		}
	}
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
	struct shm_queue_t* recvq = &(g_service.m_bind_elem->recvq);
	struct shm_queue_t* sendq = &(g_service.m_bind_elem->sendq);

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

int handle_close(int fd)
{
	fdsession_t* fdsess = get_fdsess(fd);
	if (!fdsess) {
//		ERROR_RETURN( ("connection %d had already been closed", fd), -1 );
		return -1;
	}

	assert(g_fds_session.count > 0);

	g_dll.on_cli_conn_closed(fd);

	remove_fdsess(fd);
	return 0;
}

void service_t::worker_process( int bc_elem_idx, int n_inited_bc )
{
	g_is_parent = false;
	EPOLL_TIME_OUT = 100;
	m_bind_elem = g_bind_conf.get_elem(bc_elem_idx);

	char prefix[10] = { 0 };
	int  len = snprintf(prefix, 8, "%u", m_bind_elem->id);
	prefix[len] = '_';
	log_init_ex(g_bench_conf.get_log_dir().c_str(), (E_LOG_LEVEL)g_bench_conf.get_log_level(),
		g_bench_conf.get_log_max_byte(), g_bench_conf.get_log_max_files(), prefix,
		g_bench_conf.get_log_save_next_file_interval_min());

	//释放资源(从父进程继承来的资源)
	g_shmq.close_pipe(n_inited_bc, true);
	shmq_destroy(m_bind_elem, n_inited_bc);
	net_exit();

	g_epi.init(g_bench_conf.get_max_fd_num(), 2000);//2000个数量(总FD连接数量)
	g_epi.do_add_conn(m_bind_elem->recvq.pipe_handles[0], fd_type_pipe, NULL, NULL);

	if ( 0 != handle_init(m_bind_elem)) {
		ALERT_LOG("FAIL TO INIT WORKER PROCESS. [id=%u, name=%s]", m_bind_elem->id, m_bind_elem->name.c_str());
		goto fail;
	}

	while ( !g_daemon.m_stop || 0 != handle_fini() ) {
		g_epi.loop(g_bench_conf.get_page_size());//mark
	}
fail:
	do_destroy_shmq(m_bind_elem);
	net_exit();
	log_fini();
	exit(0);
}

uint32_t service_t::get_id()
{
	return m_bind_elem->id;
}

const char* service_t::get_name()
{
	return m_bind_elem->name.c_str();
}

const char* service_t::get_ip()
{
	return m_bind_elem->ip.c_str();
}

in_port_t service_t::get_port()
{
	return m_bind_elem->port;
}
