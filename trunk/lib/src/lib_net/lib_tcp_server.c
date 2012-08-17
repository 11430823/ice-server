#include "lib_include.h"
#include "lib_file.h"
#include "lib_net/lib_net_util.h"
#include "lib_net/lib_tcp_server.h"

int ice::lib_tcp_srv_t::accept( struct sockaddr_in& peer, bool block )
{
	socklen_t peer_size = sizeof(peer);

	int newfd = HANDLE_EINTR(::accept(this->listen_fd, (struct sockaddr*)&peer, &peer_size));
	if (newfd < 0){
		return -1;
	}

	lib_file_t::set_io_block(newfd, block);

	return newfd;
}

int ice::lib_tcp_srv_t::create_passive_endpoint( const char* host, const char* serv, int backlog, int bufsize, int socktype )
{
	assert((backlog > 0) && (bufsize > 0) && (bufsize <= (10 * 1024 * 1024)));

	struct addrinfo  hints;
	struct addrinfo* res = 0;

	memset(&hints, 0, sizeof(hints));
	hints.ai_flags    = AI_PASSIVE;
	hints.ai_family   = AF_UNSPEC;
	hints.ai_socktype = socktype;

	int err = getaddrinfo(host, serv, &hints, &res);
	if (err != 0) {
		errno = lib_net_util_t::eai_to_errno(err);
		return -1;
	}

	int listenfd = ::socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (listenfd < 0) {
		freeaddrinfo(res);
		return -1;
	}

	if (SOCK_DGRAM != socktype) {
		err = lib_tcp_t::set_reuse_addr(listenfd);
		if (err == -1) {
			goto ret;
		}
	}

	err = lib_net_util_t::set_recvbuf(listenfd, bufsize);
	if (err == -1) {
		goto ret;
	}
	err = lib_net_util_t::set_sendbuf(listenfd, bufsize);
	if (err == -1) {
		goto ret;
	}

	err = ::bind(listenfd, res->ai_addr, res->ai_addrlen);
	if (err < 0) {
		goto ret;
	}

	if ((socktype == SOCK_STREAM) 
		&& (-1 == ::listen(listenfd, backlog))) {
		err = -1;
		goto ret;
	}

ret:
	if (err) {
		err      = errno;
		close(listenfd);
		listenfd = INVALID_FD;
	}
	freeaddrinfo(res);
	errno = err;

	return listenfd;
}

int ice::lib_tcp_srv_t::bind( const char* ip, uint16_t port )
{
	sockaddr_in sa_in;
	memset(&sa_in, 0, sizeof(sa_in));

	sa_in.sin_family = PF_INET;
	sa_in.sin_port = htons(port);
	if (NULL != ip){
		::inet_pton(PF_INET, ip, &sa_in.sin_addr);
	}else{
		sa_in.sin_addr.s_addr = htonl(INADDR_ANY);
	}

	return ::bind(this->listen_fd, (sockaddr*)&sa_in,sizeof(sa_in));
}

ice::lib_tcp_srv_t::lib_tcp_srv_t()
{
	this->cli_time_out_sec = 0;
	this->peer_fd_infos = NULL;
	this->listen_fd = INVALID_FD;
	this->cli_fd_value_max = 0;
}

ice::lib_tcp_srv_t::~lib_tcp_srv_t()
{
	if (NULL != this->peer_fd_infos){
		for (int i = 0; i < this->cli_fd_value_max; i++) {
			lib_tcp_peer_info_t& cfi = this->peer_fd_infos[i];
			if (FD_TYPE_UNUSED == cfi.get_fd_type()){
				continue;
			}
			cfi.close();
		}
		SAFE_DELETE_ARR(this->peer_fd_infos);
	}
	lib_file_t::close_fd(this->listen_fd);
}

ice::on_functions_tcp_srv::on_functions_tcp_srv()
{
	this->on_events = 0;
	this->on_cli_pkg = 0;
	this->on_srv_pkg = 0;
	this->on_cli_conn_closed = 0;
	this->on_svr_conn_closed = 0;
	this->on_init = 0;
	this->on_fini = 0;
	this->on_get_pkg_len = 0;
	this->on_mcast_pkg = 0;
	this->on_addr_mcast_pkg = 0;
}
