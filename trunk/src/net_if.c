#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#include <lib_file.h>
#include <lib_tcp.h>

#include "mcast.h"
#include "service.h"
#include "net_if.h"
#include "shmq.h"
#include "bind_conf.h"
#include "bench_conf.h"


static int g_ip_resolved;
static ip_port_t g_ip_port;

int connect_to_svr(const char* ipaddr, in_addr_t port, int bufsz, int timeout)
{
	struct sockaddr_in peer;
	int fd;

	memset(&peer, 0, sizeof(peer));
	peer.sin_family  = AF_INET;
	peer.sin_port    = htons(port);
	if (inet_pton(AF_INET, ipaddr, &peer.sin_addr) <= 0) {
//		ERROR_RETURN(("inet_pton %s failed, %m", ipaddr), -1);
	}

	fd = ice::lib_tcp_cli_t::safe_tcp_connect(ipaddr, port, timeout, false);
	if (fd != -1) {
//		DEBUG_LOG("CONNECTED TO\t[%s:%u fd=%d]", ipaddr, port, fd);
		g_epi.do_add_conn(fd, fd_type_remote, &peer, 0);
	} else {
//		ERROR_LOG("failed to connect to %s:%u, err=%d %s", ipaddr, port, errno, strerror(errno));
	}

	return fd;
}

int connect_to_service(const char* service_name, uint32_t svr_id, int bufsz, int timeout)
{
//	INFO_LOG("TRY CONNECTING TO\t[name=%s id=%u]", service_name, svr_id);

	addr_node_t* n = get_service_ipport(service_name, svr_id);
	if (n) {
//		INFO_LOG("SERVICE RESOLVED\t[name=%s id=%u %u ip=%s port=%d]",
//					service_name, svr_id, n->svr_id, n->ip, n->port);
		int fd = connect_to_svr(n->ip, n->port, bufsz, timeout);
		if (fd != -1) {
//			INFO_LOG("CONNECTED TO\t[%s:%u fd=%d]", n->ip, n->port, fd);
		}

		// for get_last_connecting_service
		g_ip_resolved = 1;
		memcpy(g_ip_port.ip, n->ip, sizeof(g_ip_port.ip));
		g_ip_port.port = n->port;

		return fd;
	}

	g_ip_resolved = 0;
//	ERROR_LOG("no server with the name [%s] and server id [%u] is found", service_name, svr_id);
	return -1;
}

int asyn_connect_to_svr(const char* ipaddr, in_addr_t port, int bufsz, void (*callback)(int fd, void* arg), void* arg)
{
	struct sockaddr_in peer;

	memset(&peer, 0, sizeof(peer));
	peer.sin_family  = AF_INET;
	peer.sin_port    = htons(port);
	if (inet_pton(AF_INET, ipaddr, &peer.sin_addr) <= 0) {
// 		ERROR_LOG("failed to connect to %s:%u, err=%d (inet_pton: %s)",
// 					ipaddr, port, errno, strerror(errno));
		return -1;
	}

	// create socket
	int sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
// 		ERROR_LOG("failed to connect to %s:%u, err=%d (socket: %s)",
// 					ipaddr, port, errno, strerror(errno));
		return -1;
	}
	ice::lib_file_t::set_io_block(sockfd, false);

	// connect to svr
	int done = 1;
connect_again:
	if (connect(sockfd, (const struct sockaddr*)&peer, sizeof(peer)) < 0) {
		switch (errno) {

		case EINPROGRESS:
			done = 0;
			break;
		case EINTR:
			goto connect_again;
		default:
// 			ERROR_LOG("failed to connect to %s:%u, err=%d (connect: %s)",
// 						ipaddr, port, errno, strerror(errno));
			close(sockfd);
			return -1;
		}
	}

	assert(done == 0);
//	if (done) {
// try best to avoid calling 'callback' inside function asyn_connect_to_svr 
//		DEBUG_LOG("CONNECTED TO\t[%s:%u fd=%d]", ipaddr, port, sockfd);
//		g_epi.do_add_conn(sockfd, fd_type_remote, &peer, 0);
//		callback(sockfd, arg);
//		return 1; // if we could avoding calling 'callback' here, the return value is always 0 or -1
//	}

	g_epi.do_add_conn(sockfd, fd_type_asyn_connect, &peer, 0);	
	g_epi.m_fds[sockfd].callback = callback;
	g_epi.m_fds[sockfd].arg 	 = arg;
//	DEBUG_LOG("ASYNC CONNECT TO[fd=%d id=%u]", sockfd, epi.fds[sockfd].id);

	return 0;
}

int asyn_connect_to_service(const char* service_name, uint32_t svr_id, int bufsz, void (*callback)(int fd, void* arg), void* arg)
{
//	INFO_LOG("TRY ASYNCHRONOUSLY CONNECTING TO\t[name=%s id=%u]", service_name, svr_id);

	addr_node_t* n = get_service_ipport(service_name, svr_id);
	if (n) {
// 		INFO_LOG("SERVICE RESOLVED\t[name=%s id=%u %u ip=%s port=%d]",
// 					service_name, svr_id, n->svr_id, n->ip, n->port);
		// for get_last_connecting_service
		g_ip_resolved = 1;
		memcpy(g_ip_port.ip, n->ip, sizeof(g_ip_port.ip));
		g_ip_port.port = n->port;
		// connect to a server asynchronously
		return asyn_connect_to_svr(n->ip, n->port, bufsz, callback, arg);
	}

	g_ip_resolved = 0;
//	ERROR_LOG("no server with the name [%s] and server id [%u] is found", service_name, svr_id);
	return -1;
}

void close_svr(int svrfd)
{
	do_del_conn(svrfd, 2);
}

int create_udp_socket(struct sockaddr_in* addr, const char* ip, in_port_t port)
{
	memset(addr, 0, sizeof(*addr));

	addr->sin_family = AF_INET;
	addr->sin_port   = htons(port);
	if ( inet_pton(AF_INET, ip, &(addr->sin_addr)) <= 0 ) {
		return -1;
	}

	return socket(PF_INET, SOCK_DGRAM, 0);
}

const char* resolve_service_name(const char* service_name, uint32_t svr_id)
{
	addr_node_t* n = get_service_ipport(service_name, svr_id);
	if (n) {
		return n->ip;
	}

	return 0;
}

const ip_port_t* get_last_connecting_service()
{
	if (g_ip_resolved) {
		return &g_ip_port;
	}

	return 0;
}

int net_send(int fd, const void* data, uint32_t len)
{
	int prev_stat = 0;
	int send_bytes;

	//tcp linger send
	if (g_epi.m_fds[fd].cb.sendlen > 0) {
		if (do_write_conn(fd) == -1) {
			do_del_conn(fd, g_is_parent);
			return -1;
		}
		prev_stat = 1;
	}

	send_bytes = 0;
	if (g_epi.m_fds[fd].cb.sendlen == 0) {
		send_bytes = ice::lib_tcp_t::safe_tcp_send_n(fd, data, len);
		if (send_bytes == -1) {
//			ERROR_LOG("failed to write to fd=%d err=%d %s", fd, errno, strerror(errno));
			do_del_conn(fd, g_is_parent);
			return -1;
		}
	}

	//merge buffer
	if ((int32_t)len > send_bytes){
		if (!g_epi.m_fds[fd].cb.sendptr) {
			g_epi.m_fds[fd].cb.sendptr = (uint8_t*) malloc (len - send_bytes);
			if (!g_epi.m_fds[fd].cb.sendptr)
//				ERROR_RETURN (("malloc error, %s", strerror(errno)), -1);
			g_epi.m_fds[fd].cb.sndbufsz = len - send_bytes;
			
		} else if (g_epi.m_fds[fd].cb.sndbufsz < g_epi.m_fds[fd].cb.sendlen + len - send_bytes) {
			g_epi.m_fds[fd].cb.sendptr = (uint8_t*)realloc (g_epi.m_fds[fd].cb.sendptr,
					g_epi.m_fds[fd].cb.sendlen + len - send_bytes);
			if (!g_epi.m_fds[fd].cb.sendptr)
//				ERROR_RETURN (("realloc error, %s", strerror(errno)), -1);
			g_epi.m_fds[fd].cb.sndbufsz = g_epi.m_fds[fd].cb.sendlen + len - send_bytes;
		}
			
		memcpy(g_epi.m_fds[fd].cb.sendptr + g_epi.m_fds[fd].cb.sendlen, (char*)data + send_bytes, len - send_bytes);
		g_epi.m_fds[fd].cb.sendlen += len - send_bytes;
		if (g_is_parent && (g_bench_conf.get_page_size_max() > 0)
				&& (g_epi.m_fds[fd].cb.sendlen > g_bench_conf.get_page_size_max())) {
// 			ERROR_LOG("send buf limit exceeded: fd=%d buflen=%u limit=%u",
// 						fd, epi.fds[fd].cb.sendlen, g_send_buf_limit_size);
			do_del_conn(fd, g_is_parent);
			return -1;
		}
	}

	if (g_epi.m_fds[fd].cb.sendlen > 0 && !prev_stat) 
		mod_events (g_epi.m_fd, fd, EPOLLOUT | EPOLLIN);
	else if (prev_stat && g_epi.m_fds[fd].cb.sendlen == 0)
		mod_events (g_epi.m_fd, fd, EPOLLIN);

	return 0;
}

int send_pkg_to_client(fdsession_t* fdsess, const void* pkg, const int pkglen)
{
	shm_block_t mb;

	mb.id   = fdsess->id;
	mb.fd   = fdsess->fd;
	mb.type = DATA_BLOCK;

	int send_bytes, cur_len;
	for (send_bytes = 0; send_bytes < pkglen; send_bytes += cur_len) {
		if ((pkglen - send_bytes) > (int32_t)(g_bench_conf.get_page_size_max() - sizeof(shm_block_t))) {
			cur_len = g_bench_conf.get_page_size_max() - sizeof(shm_block_t);
		} else {
			cur_len = pkglen - send_bytes;
		}

		mb.length = cur_len + sizeof(shm_block_t);

		if (shmq_push(&(g_service.m_bind_elem->sendq), &mb, (char*)pkg + send_bytes) == -1) {
			return -1;
		}
	}

	return 0;	
}

void close_client_conn(int fd)
{
	shm_block_t mb;

	fdsession_t* fdsess = get_fdsess(fd);
	if (!fdsess) {
		return;
	}

	mb.id     = fdsess->id;
	mb.length = sizeof(shm_block_t);
	mb.type   = FIN_BLOCK;
	mb.fd     = fd;

	handle_close(fd);
	shmq_push(&(g_service.m_bind_elem->sendq), &mb, 0);
}

