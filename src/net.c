#include <time.h>
#include <stdint.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/mman.h>
#include <assert.h>
#include <stdio.h>

#include <ice_lib/log.h>
#include <ice_lib/timer.h>
#include <ice_lib/lib_util.h>
#include <ice_lib/lib_tcp.h>


#include "net.h"
#include "tcp.h"
#include "service.h"
#include "ice_dll.h"
#include "net_if.h"
#include "daemon.h"
#include "mcast.h"
#include "shmq.h"
#include "bind_conf.h"


net_t g_net;

time_t socket_timeout = 30;
const uint32_t PAGE_SIZE      = 8192;
uint32_t g_send_buf_limit_size = 8192;

enum {
	trash_size		= 4096,
	mcast_pkg_size	= 8192,
	udp_pkg_size	= 8192
};

namespace {
	inline void add_to_close_queue(int fd)
	{
		del_from_etin_queue (fd);
		if (!(g_epi.m_fds[fd].flag & CN_NEED_CLOSE)) {
			list_add_tail (&g_epi.m_fds[fd].list, &g_epi.m_close_head);
			g_epi.m_fds[fd].flag |= CN_NEED_CLOSE;
			// 		TRACE_LOG("add fd=%d to close queue, %x", fd, epi.fds[fd].flag);
		}
	}

	static int schedule_output(shm_block_t *mb)
	{
		int data_len;
		int fd = mb->fd;

		if (unlikely((fd > g_epi.m_max_fd) || (fd < 0))) {
			DEBUG_LOG("discard the message: mb->type=%d, fd=%d, maxfd=%d, id=%u", 
				mb->type, fd, g_epi.m_max_fd, mb->id);
			return -1;
		}

		if (g_epi.m_fds[fd].type != fd_type_remote || mb->id != g_epi.m_fds[fd].id) { 
			TRACE_LOG ("connection %d closed, discard %u, %u block", fd, mb->id, g_epi.m_fds[fd].id);
			return -1;
		}

		if (mb->type == FIN_BLOCK && g_epi.m_fds[fd].type != fd_type_listen) {
			add_to_close_queue (fd);
			return 0;
		}

		//shm block send
		data_len = mb->length - sizeof (shm_block_t);
		return net_send(fd, mb->data, data_len);
	}

	inline void handle_send_queue()
	{
		shm_block_t *mb;
		shm_queue_t *q;

		for ( uint32_t i = 0; i != g_bind_conf.get_elem_num(); ++i ) {
			q = &(g_bind_conf.get_elem(i)->sendq);
			while (0 == shmq_pop(q, &mb)) {
				schedule_output(mb);	
			}
		}
	}
	//************************************
	// Brief:     处理管道事件(父/子进程crashed)
	// Returns:   int
	// Parameter: int fd
	// Parameter: int pos
	// Parameter: int is_conn 1:父进程. 0:子进程
	//************************************
	int handle_pipe_event(int fd, int pos, int is_conn)
	{
		char trash[trash_size];

		if (g_epi.m_evs[pos].events & EPOLLHUP) {
			if (is_conn) { // Child Crashed
				int pfd = g_epi.m_evs[pos].data.fd;
				bind_config_elem_t* bc = g_epi.m_fds[pfd].bc_elem;
				CRIT_LOG("CHILD PROCESS CRASHED![olid=%u olname=%s]", bc->id, bc->name.c_str());
				char buf[100];
				snprintf(buf, sizeof(buf), "%s.%s", bc->name.c_str(), "child.core");
				//  [9/12/2011 meng]
#if 0
				asynsvr_send_warning(buf, bc->online_id, bc->bind_ip);
#endif
				// close all connections that are related to the crashed child process
				for (int i = 0; i <= g_epi.m_max_fd; ++i) {
					if ((g_epi.m_fds[i].bc_elem == bc) && (g_epi.m_fds[i].type != fd_type_listen)) {
						//todo 
						do_del_conn(i, is_conn);
						//todo end
					}
				}
				// prevent child process from being restarted again and again forever
				if (bc->restart_cnt++ < 20) {
					restart_child_process(bc);
				}
			} else { // Parent Crashed
				CRIT_LOG("PARENT PROCESS CRASHED!");

				char buf[100];
				snprintf(buf, sizeof(buf), "%s.%s", g_service.get_name(), "parent.core");
				//  [9/12/2011 meng]	
#if 0
				asynsvr_send_warning(buf, 0, get_server_ip());
#endif
				g_daemon.m_stop = true;
				return -1;
			}
		} else {
			while (trash_size == read(fd, trash, trash_size)) ;
		}

		return 0;
	}
	void handle_asyn_connect(int fd)
	{
		fdinfo_t* fdinfo = &(g_epi.m_fds[fd]);

		int error;
		socklen_t len = sizeof(error);
		if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
			error = errno;
			ERROR_LOG("should be impossible: fd=%d id=%u err=%d (%s)",
				fd, fdinfo->id, error, strerror(error));
		}

		if (!error) {
			fdinfo->type = fd_type_remote;
			mod_events(g_epi.m_fd, fd, EPOLLIN);
			DEBUG_LOG("ASYNC CONNECTED TO[fd=%d id=%u]", fd, fdinfo->id);
		} else {
			ERROR_LOG("failed to connect to fd=%d id=%u err=%d (%s)",
				fd, fdinfo->id, error, strerror(error));
			do_del_conn(fd, 2);
			fd = -1;
		}

		fdinfo->callback(fd, fdinfo->arg);
	}
}//end of namespace

int net_start(const char* listen_ip, in_port_t listen_port, bind_config_elem_t* bc_elem)
{
	int ret_code = -1;

	int listenfd = safe_socket_listen(listen_ip, listen_port, SOCK_STREAM, 1024, 32 * 1024);
	if (-1 != listenfd) {
		//set nonblock
		lib_tcp::set_io_block(listenfd, false);
		lib_tcp::set_io_block(listenfd, false);

		g_epi.do_add_conn(listenfd, fd_type_listen, 0, bc_elem);
		ret_code = 0;
	}

	BOOT_LOG(ret_code, "Listen on %s:%u,ret_code:%d", listen_ip ? listen_ip : "ANYADDR", listen_port, ret_code);
	return ret_code;
}

inline void free_cb(struct conn_buf_t *p)
{
	if (p->sendptr) {
		free (p->sendptr);
		p->sendptr = NULL;
	}
	if (p->recvptr) {
		munmap (p->recvptr, PAGE_SIZE);//mark
		p->recvptr = NULL;
	}

	p->recvlen = 0;
	p->sendlen = 0;
	p->rcvprotlen = 0;
	p->sndbufsz = 0;
} 

void net_exit ()
{
	for (int i = 0; i < g_epi.m_max_fd + 1; i++) {
		if (g_epi.m_fds[i].type == fd_type_unused){
			continue;
		}
		free_cb (&g_epi.m_fds[i].cb);
		close (i);
	}

	free (g_epi.m_fds);
	free (g_epi.m_evs);
	close (g_epi.m_fd);
}
int do_write_conn(int fd)
{
	int send_bytes;

	send_bytes = safe_tcp_send_n(fd, g_epi.m_fds[fd].cb.sendptr, g_epi.m_fds[fd].cb.sendlen);
	if (send_bytes == 0) {
		return 0;
	} else if (send_bytes > 0) {
		if ((uint32_t)send_bytes < g_epi.m_fds[fd].cb.sendlen) {
			memmove(g_epi.m_fds[fd].cb.sendptr, g_epi.m_fds[fd].cb.sendptr + send_bytes, 
				g_epi.m_fds[fd].cb.sendlen - send_bytes);
		}

		g_epi.m_fds[fd].cb.sendlen -= send_bytes;
		g_epi.m_fds[fd].sk.last_tm = time(0);
	} else {
//		ERROR_LOG("failed to write to fd=%d err=%d %s", fd, errno, strerror(errno));
		return -1;
	}

	return send_bytes;
}
inline void del_from_close_queue (int fd)
{
	if (g_epi.m_fds[fd].flag & CN_NEED_CLOSE) {
		g_epi.m_fds[fd].flag &= ~CN_NEED_CLOSE;
		list_del_init (&g_epi.m_fds[fd].list);
	}
}
void do_del_conn(int fd, bool is_conn)
{
	if (g_epi.m_fds[fd].type == fd_type_unused)
		return ;

	if (!is_conn) {
		g_dll.on_fd_closed(fd);
	} else if (is_conn){
		shm_block_t mb;
		mb.id = g_epi.m_fds[fd].id;
		mb.fd = fd;
		mb.type = CLOSE_BLOCK;
		mb.length = sizeof (mb);
		shmq_push(&(g_epi.m_fds[fd].bc_elem->recvq), &mb, NULL);
	}

	del_from_etin_queue(fd);
	del_from_close_queue(fd);

	free_cb (&g_epi.m_fds[fd].cb);
	g_epi.m_fds[fd].type = fd_type_unused;

	//epoll will auto clear epoll events when fd closed
	close (fd);
	g_epi.m_fd_count--;

	if (g_epi.m_max_fd == fd) {
		int i;
		for (i = fd - 1; i >= 0; i--)
			if (g_epi.m_fds[i].type != fd_type_unused)
				break;
		g_epi.m_max_fd = i;
	}
	TRACE_LOG ("close fd=%d", fd);
}

inline void iterate_close_queue()
{
	struct list_head *l, *p;
	struct fdinfo_t *fi;

	list_for_each_safe (p, l, &g_epi.m_close_head) {
		fi = list_entry (p, struct fdinfo_t, list);
		if (fi->cb.sendlen > 0) {
			do_write_conn(fi->sockfd);
		}
		do_del_conn(fi->sockfd, g_is_parent ? 2 : 0);
	}
}
inline void add_to_etin_queue (int fd)
{
	if (!(g_epi.m_fds[fd].flag & (CN_NEED_CLOSE | CN_NEED_POLLIN))) {
		list_add_tail (&g_epi.m_fds[fd].list, &g_epi.m_etin_head);
		g_epi.m_fds[fd].flag |= CN_NEED_POLLIN;
//		TRACE_LOG ("add fd=%d to etin queue", fd);
	}
}
static int do_open_conn(int fd, int isconn)
{
	struct sockaddr_in peer;
	int newfd;

	newfd = safe_tcp_accept(fd, &peer, 1);
	if (newfd != -1) {
		g_epi.do_add_conn(newfd, fd_type_remote, &peer, g_epi.m_fds[fd].bc_elem);
		g_epi.m_fds[newfd].sk.last_tm = time(0);

		if (isconn) {
			shm_block_t mb;

			mb.id = g_epi.m_fds[newfd].id;
			mb.fd = newfd;
			mb.type = OPEN_BLOCK;
			mb.length = sizeof (mb) + sizeof (struct skinfo_t);
			if (shmq_push(&(g_epi.m_fds[newfd].bc_elem->recvq), &mb, (const uint8_t *)&g_epi.m_fds[newfd].sk) == -1)
				do_del_conn(newfd, 2);
		}
	} else if ((errno == EMFILE) || (errno == ENFILE)) {
		add_to_etin_queue(fd);
	} else if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
		del_from_etin_queue(fd);
	}

	return newfd;
}

static int do_read_conn(int fd, uint32_t max)
{
	int recv_bytes;

	if (NULL == g_epi.m_fds[fd].cb.recvptr) {
		g_epi.m_fds[fd].cb.rcvprotlen = 0;
		g_epi.m_fds[fd].cb.recvlen = 0;
		g_epi.m_fds[fd].cb.recvptr = (uint8_t*)mmap (0, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if (MAP_FAILED == g_epi.m_fds[fd].cb.recvptr){ 
			ERROR_LOG("MMAP FAILED");
			return -1;
		}
	}

	if (PAGE_SIZE == g_epi.m_fds[fd].cb.recvlen) {
		TRACE_LOG ("recv buffer is full, fd=%d", fd);
		return 0;
	}

	recv_bytes = safe_tcp_recv(fd, g_epi.m_fds[fd].cb.recvptr + g_epi.m_fds[fd].cb.recvlen, 
		max - g_epi.m_fds[fd].cb.recvlen);
	if (recv_bytes > 0) {
		g_epi.m_fds[fd].cb.recvlen += recv_bytes;
		g_epi.m_fds[fd].sk.last_tm  = time(0);
		//close
	} else if (recv_bytes == 0) {
		ERROR_LOG("connection [fd=%d ip=0x%X] closed by peer", fd, g_epi.m_fds[fd].sk.remote_ip);
		return -1;
	} else { //EAGAIN ...
		ERROR_LOG("recv error: fd=%d errmsg=%s", fd, strerror(errno));
		recv_bytes = 0;
	}

	if (g_epi.m_fds[fd].cb.recvlen == max) {
		add_to_etin_queue(fd);
	} else {
		del_from_etin_queue(fd);
	}

	return recv_bytes;
}
static int net_recv(int fd, uint32_t max, int is_conn)
{
	int cnt = 0;
	assert (max <= PAGE_SIZE);
	if (g_epi.m_fds[fd].type == fd_type_pipe) {
		read (fd, g_epi.m_fds[fd].cb.recvptr, max);
		return 0;
	}
	if (do_read_conn(fd, max) == -1) {
		return -1;
	}

	uint8_t* saved_recvptr = g_epi.m_fds[fd].cb.recvptr;
parse_again:	
	//unknow protocol length
	if (g_epi.m_fds[fd].cb.rcvprotlen == 0) {
		//parse
		g_epi.m_fds[fd].cb.rcvprotlen = g_dll.on_get_pkg_len(fd, (void*)g_epi.m_fds[fd].cb.recvptr, g_epi.m_fds[fd].cb.recvlen, is_conn);
// 		TRACE_LOG("handle_parse pid=%d return %d, buffer len=%d, fd=%d", getpid(),
// 			epi.fds[fd].cb.rcvprotlen, epi.fds[fd].cb.recvlen, fd);
	}

	//invalid protocol length
	if (unlikely(g_epi.m_fds[fd].cb.rcvprotlen > max)) {
		g_epi.m_fds[fd].cb.recvptr = saved_recvptr;
		return -1;
		//unknow protocol length
	} else if (unlikely(g_epi.m_fds[fd].cb.rcvprotlen == 0)) {
		if (g_epi.m_fds[fd].cb.recvlen == max) {
			g_epi.m_fds[fd].cb.recvptr = saved_recvptr;
//			ERROR_RETURN(("unsupported big protocol, recvlen=%d", epi.fds[fd].cb.recvlen), -1);
			return -1;
		}
		//integrity protocol	
	} else if (g_epi.m_fds[fd].cb.recvlen >= g_epi.m_fds[fd].cb.rcvprotlen) {
		if (!is_conn) {
			g_dll.on_srv_pkg(fd, g_epi.m_fds[fd].cb.recvptr, g_epi.m_fds[fd].cb.rcvprotlen);
		} else {
			shm_block_t mb;
			epi2shm(fd, &mb);
			if (shmq_push(&g_epi.m_fds[fd].bc_elem->recvq, &mb, g_epi.m_fds[fd].cb.recvptr)) {
				g_epi.m_fds[fd].cb.recvptr = saved_recvptr;
				return -1;
			}
		}

		cnt++;
		if (g_epi.m_fds[fd].cb.recvlen > g_epi.m_fds[fd].cb.rcvprotlen) {
			g_epi.m_fds[fd].cb.recvptr += g_epi.m_fds[fd].cb.rcvprotlen;
		}
		g_epi.m_fds[fd].cb.recvlen    -= g_epi.m_fds[fd].cb.rcvprotlen;
		g_epi.m_fds[fd].cb.rcvprotlen  = 0;
		if (g_epi.m_fds[fd].cb.recvlen > 0) 
			goto parse_again;
	}

	if (g_epi.m_fds[fd].cb.recvptr != saved_recvptr) {
		if (g_epi.m_fds[fd].cb.recvlen) {
			memmove(saved_recvptr, g_epi.m_fds[fd].cb.recvptr, g_epi.m_fds[fd].cb.recvlen);
		}
		g_epi.m_fds[fd].cb.recvptr = saved_recvptr;
	}

	return cnt;
}
inline void iterate_etin_queue(int max_len, int is_conn)
{
	struct list_head *l, *p;
	struct fdinfo_t *fi;

	list_for_each_safe (p, l, &g_epi.m_etin_head) {
		fi = list_entry(p, struct fdinfo_t, list);
		if (unlikely(fi->type == fd_type_listen)) {
			//accept
			while (do_open_conn(fi->sockfd, is_conn) > 0) ;
		} else if (net_recv(fi->sockfd, max_len, is_conn) == -1) {
			do_del_conn(fi->sockfd, is_conn);
		}
	}
}

int mod_events(int epfd, int fd, uint32_t flag)
{
	struct epoll_event ev;

	ev.events = EPOLLET | flag;
	ev.data.fd = fd;

epoll_mod_again:
	if (unlikely (epoll_ctl (epfd, EPOLL_CTL_MOD, fd, &ev) != 0)) {
//		ERROR_LOG ("epoll_ctl mod %d error: %m", fd);
		if (errno == EINTR)
			goto epoll_mod_again;
		return -1;
	}

	return 0;
}

int net_loop(int timeout, int max_len, int is_conn)
{
	iterate_close_queue();
	iterate_etin_queue(max_len, is_conn);

	int nr = epoll_wait(g_epi.m_fd, g_epi.m_evs, g_epi.m_max_ev_num, timeout);
	if (unlikely(nr < 0 && errno != EINTR)){
		ALERT_LOG("EPOLL_WAIT FAILED, [maxfd=%d, epfd=%d]", g_epi.m_max_fd, g_epi.m_fd);
		return -1;
	}

	renew_now();
	//todo 检查
	if (is_conn) {
		handle_send_queue();
	}	//todo end

	for (int pos = 0; pos < nr; pos++) {
		int fd = g_epi.m_evs[pos].data.fd;

		if (fd > g_epi.m_max_fd || g_epi.m_fds[fd].sockfd != fd || g_epi.m_fds[fd].type == fd_type_unused) {
 			ERROR_LOG("DELAYED EPOLL EVENTS [event fd=%d, cache fd=%d, maxfd=%d, type=%d]", 
 				fd, g_epi.m_fds[fd].sockfd, g_epi.m_max_fd, g_epi.m_fds[fd].type);
			continue;
		}

		if ( unlikely(fd_type_pipe == g_epi.m_fds[fd].type ) ) {
			if (0 == handle_pipe_event(fd, pos, is_conn)) {
				continue;
			} else {
				return -1;
			}
		}

		if ( unlikely(fd_type_asyn_connect == g_epi.m_fds[fd].type) ) {
			handle_asyn_connect(fd);
			continue;
		}

		if (g_epi.m_evs[pos].events & EPOLLIN) {
			switch (g_epi.m_fds[fd].type) {
			case fd_type_listen:
				//accept
				while (do_open_conn(fd, is_conn) > 0) ;
				break;
			case fd_type_mcast:
				{
					static char buf[mcast_pkg_size];
					int  i;
					for (i = 0; i != 100; ++i) {
						int len = recv(fd, buf, mcast_pkg_size, MSG_DONTWAIT);
						if (len > 0) {
							if (g_dll.on_mcast_pkg) {
								g_dll.on_mcast_pkg((void*)buf, len);
							}
						} else {
							break;
						}
					}
					break;
				}
			case fd_type_addr_mcast:
				{
					static char buf[mcast_pkg_size];
					int  i;
					for (i = 0; i != 100; ++i) {
						int len = recv(fd, buf, mcast_pkg_size, MSG_DONTWAIT);
						if (len > 0) {
							asyncserv_proc_mcast_pkg(buf, len);
						} else {
							break;
						}
					}
					break;
				}
			case fd_type_udp:
				{
					static char buf[udp_pkg_size];
					int  i;
					for (i = 0; i != 100; ++i) {
						struct sockaddr_in from; 
						socklen_t fromlen;
						int len = recvfrom(fd, buf, udp_pkg_size, MSG_DONTWAIT,
							(struct sockaddr*)(&from), &fromlen);
						if (len > 0) {
							g_dll.on_udp_pkg(fd, buf, len, &from, fromlen);
						} else {
							break;
						}
					}
					break;
				}

			default:
				if (net_recv(fd, max_len, is_conn) == -1) {
					do_del_conn(fd, is_conn);
				}
				break;
			}
		}

		if (g_epi.m_evs[pos].events & EPOLLOUT) {
			if (g_epi.m_fds[fd].cb.sendlen > 0 && do_write_conn(fd) == -1) {
				do_del_conn(fd, is_conn);
			}
			if (g_epi.m_fds[fd].cb.sendlen == 0) {
				mod_events(g_epi.m_fd, fd, EPOLLIN);
			}
		}

		if (g_epi.m_evs[pos].events & EPOLLHUP) {
			do_del_conn(fd, is_conn);
		}
	}

	if (is_conn && socket_timeout) {
		int i;
		for (i = 0; i <= g_epi.m_max_fd; ++i) {
			if ((g_epi.m_fds[i].type == fd_type_remote)
				&& ((time(0) - g_epi.m_fds[i].sk.last_tm) >= socket_timeout)) {
					do_del_conn(i, is_conn);
			}
		}
	}
	if(!is_conn) {
		g_dll.on_events();

		handle_recv_queue();

		time_t now_sec = time(0);
		// syn addr
		if (now_sec > next_syn_addr_tm) {
			send_addr_mcast_pkg(addr_mcast_syn_pkg);
		}
		// del expired addrs
		if (now_sec > next_del_addrs_tm) {
			del_expired_addrs();
		}
	}

	return 0;
}

inline void del_from_etin_queue (int fd)
{
	if (g_epi.m_fds[fd].flag & CN_NEED_POLLIN) {
		g_epi.m_fds[fd].flag &= ~CN_NEED_POLLIN;
		list_del_init (&g_epi.m_fds[fd].list);
//		TRACE_LOG ("del fd=%d from etin queue", fd);
	}
}

int net_t::init( int size, int maxevents )
{
	if ((g_epi.m_fd = epoll_create(maxevents)) < 0) {
		ALERT_LOG("EPOLL_CREATE FAILED [error:%s]", strerror (errno));
		return -1;
	}

	g_epi.m_evs = (epoll_event*)calloc(maxevents, sizeof(struct epoll_event));
	if (!g_epi.m_evs) {
		close (g_epi.m_fd);
		ALERT_LOG ("CALLOC EPOLL_EVENT FAILED [size=%d]", maxevents);
		return -1;
	}

	g_epi.m_fds = (struct fdinfo_t*) calloc (size, sizeof (struct fdinfo_t));
	if (!g_epi.m_fds){
		free (g_epi.m_evs);
		close (g_epi.m_fd);
		ALERT_LOG ("CALLOC FDINFO_T FAILED [size=%d]", size);
		return -1;
	}

	g_epi.m_max_ev_num = maxevents;
	g_epi.m_max_fd = 0;
	g_epi.m_fd_count = 0;
	INIT_LIST_HEAD (&g_epi.m_etin_head);
	INIT_LIST_HEAD (&g_epi.m_close_head);
	return 0;
}
