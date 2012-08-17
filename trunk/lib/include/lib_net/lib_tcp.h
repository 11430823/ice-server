/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	ok
	brief:		
*********************************************************************/

#pragma once

#include "lib_include.h"
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

		virtual int send(const void* data, int len);

		virtual int recv(void* data, int len);

		std::string get_ip_str();

		/**
		* @brief Create a TCP connection
		*
		* @param const std::string& ip,  the ip address to connect to.
		* @param uint16_t port,  the port to connect to.
		* @param int timeout,  abort the connecting attempt after timeout secs. If timeout is less than or equal to 0, 
		*                                then the connecting attempt will not be interrupted until error occurs.
		* @param int block,  true and the connected fd will be set blocking, false and the fd will be set nonblocking.
		* @param uint32_t send_buf_len, 发送缓冲大小
		* @param uint32_t rev_buf_len, 接收缓冲大小
		* @return int, the connected fd on success, -1 on error.
		*/
		// todo need test connect timeout [3/11/2012 meng]
		static int connect( const std::string& ip, uint16_t port, int timeout, bool block, uint32_t send_buf_len = 0, uint32_t rev_buf_len = 0 );
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