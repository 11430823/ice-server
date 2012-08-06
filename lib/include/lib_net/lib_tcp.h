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
  * @brief ����һ��TCP listen socket����UDP socket�����ڽ��տͻ������ݡ�֧��IPv4��IPv6��
  *
  * @param host �����ĵ�ַ��������IP��ַ��Ҳ������������������ݲ���Ϊ��0.0.0.0���������INADDR_ANY��
  * @param serv �����Ķ˿ڡ����������ֶ˿ڣ�Ҳ�����Ƕ˿ڶ�Ӧ�ķ���������dns��time��ftp�ȡ�
  * @param socktype socket�����ͣ�������SOCK_STREAM��TCP������SOCK_DGRAM��UDP����
  * @param backlog δ������Ӷ��еĴ�С��һ����1024���ӡ�
  * @param bufsize socket����/���ͻ����С���ֽڣ�������С��10 * 1024 * 1024��
  *
  * @return �ɹ������´�����fd��ʧ�ܷ���-1��fd����ʱ����ʹ��close�رա�
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