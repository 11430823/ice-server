/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	todo
	brief:		ok
*********************************************************************/

#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "lib_util.h"
#include "lib_net.h"

namespace ice{
	class lib_tcp_t : public lib_net_t
	{
		PROTECTED_RW(uint16_t, port);
		PROTECTED_RW(uint32_t, ip);
	public:
		lib_tcp_t(){
			this->port = 0;
			this->ip = 0;
		}
		virtual ~lib_tcp_t(){}

		virtual int send(const void* buf, int total);

		virtual int recv(void* buf, int bufsize);

		std::string get_ip_str();

		/**
		* @brief Create a TCP connection
		*
		* @param const char* ip,  the ip address to connect to.
		* @param uint16_t port,  the port to connect to.
		* @param int timeout,  abort the connecting attempt after timeout secs. If timeout is less than or equal to 0, 
		*                                then the connecting attempt will not be interrupted until error occurs.
		* @param int block,  true and the connected fd will be set blocking, false and the fd will be set nonblocking.
		*
		* @return int, the connected fd on success, -1 on error.
		*/
		// todo need test connect timeout [3/11/2012 meng]
		static int connect( const char* ip, uint16_t port, int timeout, bool block );
		//************************************
		// Brief:	  Set the given fd SO_REUSEADDR
		// Returns:   int (0 on success, -1 on error)
		// Parameter: int s (fd)
		//************************************
		static int set_reuse_addr(int s);

	protected:
		
	private:
		lib_tcp_t(const lib_tcp_t& cr);
		lib_tcp_t& operator=(const lib_tcp_t& cr);
	};

}//end namespace ice


#if 0 //todo add 
/**
  * @brief 创建一个TCP listen socket或者UDP socket，用于接收客户端数据。支持IPv4和IPv6。
  *
  * @param host 监听的地址。可以是IP地址，也可以是域名。如果传递参数为“0.0.0.0”，则监听INADDR_ANY。
  * @param serv 监听的端口。可以是数字端口，也可以是端口对应的服务名，如dns、time、ftp等。
  * @param socktype socket的类型，可以是SOCK_STREAM（TCP）或者SOCK_DGRAM（UDP）。
  * @param backlog 未完成连接队列的大小。一般用1024足矣。
  * @param bufsize socket接收/发送缓冲大小（字节），必须小于10 * 1024 * 1024。
  *
  * @return 成功返回新创建的fd，失败返回-1。fd不用时必须使用close关闭。
  * @see safe_socket_listen
  */
int create_passive_endpoint(const char* host, const char* serv, int socktype, int backlog, int bufsize)
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
		errno = eai_to_errno(err);
		return -1;
	}

	int listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (listenfd < 0) {
		freeaddrinfo(res);
		return -1;
	}

	if (socktype != SOCK_DGRAM) {
		int reuse_addr = 1;	
		err = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));
		if (err == -1) {
			goto ret;
		}
	}
	err = setsockopt(listenfd, SOL_SOCKET, SO_RCVBUF, &bufsize, sizeof(int));
	if (err == -1) {
		goto ret;
	}
	err = setsockopt(listenfd, SOL_SOCKET, SO_SNDBUF, &bufsize, sizeof(int));
	if (err == -1) {
		goto ret;
	}

	err = bind(listenfd, res->ai_addr, res->ai_addrlen);
	if (err < 0) {
		goto ret;
	}

	if ((socktype == SOCK_STREAM) && (listen(listenfd, backlog) == -1)) {
		err = -1;
		goto ret;
	}

ret:
	if (err) {
		err      = errno;
		close(listenfd);
		listenfd = -1;
	}
	freeaddrinfo(res);
	errno = err;

	return listenfd;
}

/**
  * @brief Receive `total` bytes of data from `sockfd`
  *
  * @param int sockfd,  socket fd to receive data from.
  * @param const void* buf,  buffer to hold the receiving data. Size of `buf` should be no less than `total`
  * @param int total,  number of bytes to receive.
  *
  * @return int, number of bytes receive on success, -1 on error, 0 on connection closed by peer.
  */
int safe_tcp_recv_n(int sockfd, void* buf, int total)
{
	assert(total > 0);

	int recv_bytes, cur_len;

	for (recv_bytes = 0; recv_bytes < total; recv_bytes += cur_len)	{
		cur_len = recv(sockfd, buf + recv_bytes, total - recv_bytes, 0);
		// connection closed by client
		if (cur_len == 0) {
			return 0;
		} else if (cur_len == -1) {
			if (errno == EINTR) {
				cur_len = 0;
			} else if (errno == EAGAIN || errno == EWOULDBLOCK)	{
				return recv_bytes;
			} else {
				return -1;
			}
		}
	}

	return recv_bytes;
}

#endif