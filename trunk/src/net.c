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

#include "net.h"
#include "util.h"
#include "tcp.h"
#include "service.h"
#include "dll.h"
#include "net_if.h"
#include "daemon.h"
#include "mcast.h"
#include "log.h"
#include "timer.h"

net_t g_net;
epinfo epi;
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
		if (!(epi.fds[fd].flag & CN_NEED_CLOSE)) {
			list_add_tail (&epi.fds[fd].list, &epi.close_head);
			epi.fds[fd].flag |= CN_NEED_CLOSE;
			// 		TRACE_LOG("add fd=%d to close queue, %x", fd, epi.fds[fd].flag);
		}
	}

	static int schedule_output(shm_block_t *mb)
	{
		int data_len;
		int fd = mb->fd;

		if (unlikely((fd > epi.maxfd) || (fd < 0))) {
			DEBUG_LOG("discard the message: mb->type=%d, fd=%d, maxfd=%d, id=%u", 
				mb->type, fd, epi.maxfd, mb->id);
			return -1;
		}

		if (epi.fds[fd].type != fd_type_remote || mb->id != epi.fds[fd].id) { 
			TRACE_LOG ("connection %d closed, discard %u, %u block", fd, mb->id, epi.fds[fd].id);
			return -1;
		}

		if (mb->type == FIN_BLOCK && epi.fds[fd].type != fd_type_listen) {
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

		if (epi.evs[pos].events & EPOLLHUP) {
			if (is_conn) { // Child Crashed
				int pfd = epi.evs[pos].data.fd;
				bind_config_elem_t* bc = epi.fds[pfd].bc_elem;
				CRIT_LOG("CHILD PROCESS CRASHED![olid=%u olname=%s]", bc->id, bc->name.c_str());
				char buf[100];
				snprintf(buf, sizeof(buf), "%s.%s", bc->name.c_str(), "child.core");
				//  [9/12/2011 meng]
#if 0
				asynsvr_send_warning(buf, bc->online_id, bc->bind_ip);
#endif
				// close all connections that are related to the crashed child process
				for (int i = 0; i <= epi.maxfd; ++i) {
					if ((epi.fds[i].bc_elem == bc) && (epi.fds[i].type != fd_type_listen)) {
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
				snprintf(buf, sizeof(buf), "%s.%s", get_server_name(), "parent.core");
				//  [9/12/2011 meng]	
#if 0
				asynsvr_send_warning(buf, 0, get_server_ip());
#endif
				g_daemon.stop = true;
				return -1;
			}
		} else {
			while (trash_size == read(fd, trash, trash_size)) ;
		}

		return 0;
	}
	void handle_asyn_connect(int fd)
	{
		fdinfo_t* fdinfo = &(epi.fds[fd]);

		int error;
		socklen_t len = sizeof(error);
		if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
			error = errno;
			ERROR_LOG("should be impossible: fd=%d id=%u err=%d (%s)",
				fd, fdinfo->id, error, strerror(error));
		}

		if (!error) {
			fdinfo->type = fd_type_remote;
			mod_events(epi.epfd, fd, EPOLLIN);
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


static inline int add_events (int epfd, int fd, uint32_t flag)
{
	struct epoll_event ev;

	ev.events = flag;
	ev.data.fd = fd;
epoll_add_again:
	if (unlikely (0 != epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev))) {
		if (errno == EINTR){
			goto epoll_add_again;
		}else{
			ERROR_LOG ("epoll_ctl add %d error: %m, %d", fd, -1);
			return -1;
		}
	}
	return 0; 
}

int do_add_conn(int fd, uint8_t type, struct sockaddr_in *peer, bind_config_elem_t* bc_elem)
{
	static uint32_t seq = 0;
	uint32_t flag;

	switch (type) {
	case fd_type_pipe:
	case fd_type_mcast:
	case fd_type_addr_mcast:
	case fd_type_udp:
		flag = EPOLLIN;
		break;
	case fd_type_asyn_connect:
		flag = EPOLLOUT;
		break;
	default:
		flag = EPOLLIN | EPOLLET;
	break;
	}

	if (0 != add_events(epi.epfd, fd, flag)) {
		return -1;
	}

	memset(&epi.fds[fd], 0, sizeof(struct fdinfo_t));
	epi.fds[fd].sockfd = fd;
	epi.fds[fd].type = type;
	
	epi.fds[fd].id = ++seq;
	if (0 == seq) {
		epi.fds[fd].id = ++seq;
	}
	if (peer) {
		epi.fds[fd].sk.remote_ip = peer->sin_addr.s_addr;
		epi.fds[fd].sk.remote_port = peer->sin_port;
	}
	epi.fds[fd].bc_elem = bc_elem;
	epi.maxfd = epi.maxfd > fd ? epi.maxfd : fd;
	epi.count++;

	TRACE_LOG("add fd:%d, type:%d, id:%u", fd, type, epi.fds[fd].id);
	return 0;
}

int net_start(const char* listen_ip, in_port_t listen_port, bind_config_elem_t* bc_elem)
{
	int ret_code = -1;

	int listenfd = safe_socket_listen(listen_ip, listen_port, SOCK_STREAM, 1024, 32 * 1024);
	if (-1 != listenfd) {
		//set nonblock
		set_io_blockability(listenfd, 1);

		do_add_conn(listenfd, fd_type_listen, 0, bc_elem);
		ret_code = 0;
	}

	ALERT_LOG("Listen on %s:%u,ret_code:%d\r\n", listen_ip ? listen_ip : "ANYADDR", listen_port, ret_code);
	return ret_code;
}

inline void free_cb(struct conn_buf_t *p)
{
	if (p->sendptr) {
		free (p->sendptr);
		p->sendptr = NULL;
	}
	if (p->recvptr) {
		munmap (p->recvptr, PAGE_SIZE);
		p->recvptr = NULL;
	}

	p->recvlen = 0;
	p->sendlen = 0;
	p->rcvprotlen = 0;
	p->sndbufsz = 0;
} 

void net_exit ()
{
	for (int i = 0; i < epi.maxfd + 1; i++) {
		if (epi.fds[i].type == fd_type_unused){
			continue;
		}
		free_cb (&epi.fds[i].cb);
		close (i);
	}

	free (epi.fds);
	free (epi.evs);
	close (epi.epfd);
}
int do_write_conn(int fd)
{
	int send_bytes;

	send_bytes = safe_tcp_send_n(fd, epi.fds[fd].cb.sendptr, epi.fds[fd].cb.sendlen);
	if (send_bytes == 0) {
		return 0;
	} else if (send_bytes > 0) {
		if ((uint32_t)send_bytes < epi.fds[fd].cb.sendlen) {
			memmove(epi.fds[fd].cb.sendptr, epi.fds[fd].cb.sendptr + send_bytes, 
				epi.fds[fd].cb.sendlen - send_bytes);
		}

		epi.fds[fd].cb.sendlen -= send_bytes;
		epi.fds[fd].sk.last_tm = time(0);
	} else {
//		ERROR_LOG("failed to write to fd=%d err=%d %s", fd, errno, strerror(errno));
		return -1;
	}

	return send_bytes;
}
inline void del_from_close_queue (int fd)
{
	if (epi.fds[fd].flag & CN_NEED_CLOSE) {
		epi.fds[fd].flag &= ~CN_NEED_CLOSE;
		list_del_init (&epi.fds[fd].list);
	}
}
void do_del_conn(int fd, int is_conn)
{
	if (epi.fds[fd].type == fd_type_unused)
		return ;

	if (is_conn == 0) {
		g_dll.on_fd_closed(fd);
	} else if (is_conn == 1){
		shm_block_t mb;
		mb.id = epi.fds[fd].id;
		mb.fd = fd;
		mb.type = CLOSE_BLOCK;
		mb.length = sizeof (mb);
		shmq_push(&(epi.fds[fd].bc_elem->recvq), &mb, NULL);
	}

	del_from_etin_queue(fd);
	del_from_close_queue(fd);

	free_cb (&epi.fds[fd].cb);
	epi.fds[fd].type = fd_type_unused;

	//epoll will auto clear epoll events when fd closed
	close (fd);
	epi.count--;

	if (epi.maxfd == fd) {
		int i;
		for (i = fd - 1; i >= 0; i--)
			if (epi.fds[i].type != fd_type_unused)
				break;
		epi.maxfd = i;
	}
	TRACE_LOG ("close fd=%d", fd);
}

inline void iterate_close_queue()
{
	struct list_head *l, *p;
	struct fdinfo_t *fi;

	list_for_each_safe (p, l, &epi.close_head) {
		fi = list_entry (p, struct fdinfo_t, list);
		if (fi->cb.sendlen > 0) {
			do_write_conn(fi->sockfd);
		}
		do_del_conn(fi->sockfd, is_parent ? 2 : 0);
	}
}
inline void add_to_etin_queue (int fd)
{
	if (!(epi.fds[fd].flag & (CN_NEED_CLOSE | CN_NEED_POLLIN))) {
		list_add_tail (&epi.fds[fd].list, &epi.etin_head);
		epi.fds[fd].flag |= CN_NEED_POLLIN;
//		TRACE_LOG ("add fd=%d to etin queue", fd);
	}
}
static int do_open_conn(int fd, int isconn)
{
	struct sockaddr_in peer;
	int newfd;

	newfd = safe_tcp_accept(fd, &peer, 1);
	if (newfd != -1) {
		do_add_conn(newfd, fd_type_remote, &peer, epi.fds[fd].bc_elem);
		epi.fds[newfd].sk.last_tm = time(0);

		if (isconn) {
			shm_block_t mb;

			mb.id = epi.fds[newfd].id;
			mb.fd = newfd;
			mb.type = OPEN_BLOCK;
			mb.length = sizeof (mb) + sizeof (struct skinfo_t);
			if (shmq_push(&(epi.fds[newfd].bc_elem->recvq), &mb, (const uint8_t *)&epi.fds[newfd].sk) == -1)
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

	if (NULL == epi.fds[fd].cb.recvptr) {
		epi.fds[fd].cb.rcvprotlen = 0;
		epi.fds[fd].cb.recvlen = 0;
		epi.fds[fd].cb.recvptr = (uint8_t*)mmap (0, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if (MAP_FAILED == epi.fds[fd].cb.recvptr){ 
			ERROR_LOG("MMAP FAILED");
			return -1;
		}
	}

	if (PAGE_SIZE == epi.fds[fd].cb.recvlen) {
		TRACE_LOG ("recv buffer is full, fd=%d", fd);
		return 0;
	}

	recv_bytes = safe_tcp_recv(fd, epi.fds[fd].cb.recvptr + epi.fds[fd].cb.recvlen, 
		max - epi.fds[fd].cb.recvlen);
	if (recv_bytes > 0) {
		epi.fds[fd].cb.recvlen += recv_bytes;
		epi.fds[fd].sk.last_tm  = time(0);
		//close
	} else if (recv_bytes == 0) {
//		ERROR_LOG("connection [fd=%d ip=0x%X] closed by peer", fd, epi.fds[fd].sk.remote_ip);
		return -1;
	} else { //EAGAIN ...
//		ERROR_LOG("recv error: fd=%d errmsg=%s", fd, strerror(errno));
		recv_bytes = 0;
	}

	if (epi.fds[fd].cb.recvlen == max) {
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
	if (epi.fds[fd].type == fd_type_pipe) {
		read (fd, epi.fds[fd].cb.recvptr, max);
		return 0;
	}
	if (do_read_conn(fd, max) == -1) {
		return -1;
	}

	uint8_t* saved_recvptr = epi.fds[fd].cb.recvptr;
parse_again:	
	//unknow protocol length
	if (epi.fds[fd].cb.rcvprotlen == 0) {
		//parse
		epi.fds[fd].cb.rcvprotlen = g_dll.get_pkg_len(fd, (void*)epi.fds[fd].cb.recvptr, epi.fds[fd].cb.recvlen, is_conn);
// 		TRACE_LOG("handle_parse pid=%d return %d, buffer len=%d, fd=%d", getpid(),
// 			epi.fds[fd].cb.rcvprotlen, epi.fds[fd].cb.recvlen, fd);
	}

	//invalid protocol length
	if (unlikely(epi.fds[fd].cb.rcvprotlen > max)) {
		epi.fds[fd].cb.recvptr = saved_recvptr;
		return -1;
		//unknow protocol length
	} else if (unlikely(epi.fds[fd].cb.rcvprotlen == 0)) {
		if (epi.fds[fd].cb.recvlen == max) {
			epi.fds[fd].cb.recvptr = saved_recvptr;
//			ERROR_RETURN(("unsupported big protocol, recvlen=%d", epi.fds[fd].cb.recvlen), -1);
			return -1;
		}
		//integrity protocol	
	} else if (epi.fds[fd].cb.recvlen >= epi.fds[fd].cb.rcvprotlen) {
		if (!is_conn) {
			g_dll.proc_pkg_from_serv(fd, epi.fds[fd].cb.recvptr, epi.fds[fd].cb.rcvprotlen);
		} else {
			shm_block_t mb;
			epi2shm(fd, &mb);
			if (shmq_push(&epi.fds[fd].bc_elem->recvq, &mb, epi.fds[fd].cb.recvptr)) {
				epi.fds[fd].cb.recvptr = saved_recvptr;
				return -1;
			}
		}

		cnt++;
		if (epi.fds[fd].cb.recvlen > epi.fds[fd].cb.rcvprotlen) {
			epi.fds[fd].cb.recvptr += epi.fds[fd].cb.rcvprotlen;
		}
		epi.fds[fd].cb.recvlen    -= epi.fds[fd].cb.rcvprotlen;
		epi.fds[fd].cb.rcvprotlen  = 0;
		if (epi.fds[fd].cb.recvlen > 0) 
			goto parse_again;
	}

	if (epi.fds[fd].cb.recvptr != saved_recvptr) {
		if (epi.fds[fd].cb.recvlen) {
			memmove(saved_recvptr, epi.fds[fd].cb.recvptr, epi.fds[fd].cb.recvlen);
		}
		epi.fds[fd].cb.recvptr = saved_recvptr;
	}

	return cnt;
}
inline void iterate_etin_queue(int max_len, int is_conn)
{
	struct list_head *l, *p;
	struct fdinfo_t *fi;

	list_for_each_safe (p, l, &epi.etin_head) {
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
#include "timer.h"
int net_loop(int timeout, int max_len, int is_conn)
{
	//todo 检查
	iterate_close_queue();
	iterate_etin_queue(max_len, is_conn);
	//todo end

	int nr = epoll_wait(epi.epfd, epi.evs, epi.max_ev_num, timeout);
	if (unlikely(nr < 0 && errno != EINTR)){
		ALERT_LOG("EPOLL_WAIT FAILED, [maxfd=%d, epfd=%d]", epi.maxfd, epi.epfd);
		return -1;
	}

	renew_now();
	//todo 检查
	if (is_conn) {
		handle_send_queue();
	}	//todo end

	for (int pos = 0; pos < nr; pos++) {
		int fd = epi.evs[pos].data.fd;

		if (fd > epi.maxfd || epi.fds[fd].sockfd != fd || epi.fds[fd].type == fd_type_unused) {
 			ERROR_LOG("DELAYED EPOLL EVENTS [event fd=%d, cache fd=%d, maxfd=%d, type=%d]", 
 				fd, epi.fds[fd].sockfd, epi.maxfd, epi.fds[fd].type);
			continue;
		}

		if ( unlikely(fd_type_pipe == epi.fds[fd].type ) ) {
			if (0 == handle_pipe_event(fd, pos, is_conn)) {
				continue;
			} else {
				return -1;
			}
		}

		if ( unlikely(fd_type_asyn_connect == epi.fds[fd].type) ) {
			handle_asyn_connect(fd);
			continue;
		}

		if (epi.evs[pos].events & EPOLLIN) {
			switch (epi.fds[fd].type) {
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
							if (g_dll.proc_mcast_pkg) {
								g_dll.proc_mcast_pkg((void*)buf, len);
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
							g_dll.proc_udp_pkg(fd, buf, len, &from, fromlen);
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

		if (epi.evs[pos].events & EPOLLOUT) {
			if (epi.fds[fd].cb.sendlen > 0 && do_write_conn(fd) == -1) {
				do_del_conn(fd, is_conn);
			}
			if (epi.fds[fd].cb.sendlen == 0) {
				mod_events(epi.epfd, fd, EPOLLIN);
			}
		}

		if (epi.evs[pos].events & EPOLLHUP) {
			do_del_conn(fd, is_conn);
		}
	}

	if (is_conn && socket_timeout) {
		int i;
		for (i = 0; i <= epi.maxfd; ++i) {
			if ((epi.fds[i].type == fd_type_remote)
				&& ((time(0) - epi.fds[i].sk.last_tm) >= socket_timeout)) {
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
	if (epi.fds[fd].flag & CN_NEED_POLLIN) {
		epi.fds[fd].flag &= ~CN_NEED_POLLIN;
		list_del_init (&epi.fds[fd].list);
//		TRACE_LOG ("del fd=%d from etin queue", fd);
	}
}

int net_t::init( int size, int maxevents )
{
	if ((epi.epfd = epoll_create(maxevents)) < 0) {
		ALERT_LOG("EPOLL_CREATE FAILED [error:%s]", strerror (errno));
		return -1;
	}

	epi.evs = (epoll_event*)calloc(maxevents, sizeof(struct epoll_event));
	if (!epi.evs) {
		close (epi.epfd);
		ALERT_LOG ("CALLOC EPOLL_EVENT FAILED [size=%d]", maxevents);
		return -1;
	}

	epi.fds = (struct fdinfo_t*) calloc (size, sizeof (struct fdinfo_t));
	if (!epi.fds){
		free (epi.evs);
		close (epi.epfd);
		ALERT_LOG ("CALLOC FDINFO_T FAILED [size=%d]", size);
		return -1;
	}

	epi.max_ev_num = maxevents;
	epi.maxfd = 0;
	epi.count = 0;
	INIT_LIST_HEAD (&epi.etin_head);
	INIT_LIST_HEAD (&epi.close_head);
	return 0;
}
