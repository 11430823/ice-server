/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
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
	public:
		lib_tcp_t(){}
		virtual ~lib_tcp_t(){}
		virtual int send(const void* buf, int total);
		/**
		 * @brief	Receive data from fd
		 * @param	void * buf		buffer to hold the receiving data.
		 * @param	int				bufsize	number of bytes in total to be sent.
		 * @return	int				number of bytes sent on success, -1 on error.
		 */
		virtual int recv(void* buf, int bufsize);
		/**
		* @brief Send `total` bytes of data to `sockfd`
		* @param int sockfd,  socket fd to send data to.
		* @param const void* buf,  data to be sent.
		* @param int total,  number of bytes in total to be sent.
		* @return int, number of bytes sent on success, -1 on error.
		*/
		static int safe_tcp_send_n(int sockfd, const void* buf, int total);
		/**
		* @brief Receive `total` bytes of data from `sockfd`
		* @param int sockfd,  socket fd to receive data from.
		* @param const void* buf,  buffer to hold the receiving data. Size of `buf` should be no less than `total`
		* @param int total,  number of bytes to receive.
		* @return int, number of bytes receive on success, -1 on error, 0 on connection closed by peer.
		*/
		static int safe_tcp_recv_n(int sockfd, void* buf, int total);

		/**
		* @brief Set a timeout on sending data. If you want to disable timeout, just simply
		*           call this function again with millisec set to be 0.
		*
		* @param sockfd socket descriptor to be set.
		* @param millisec timeout in milliseconds.
		*
		* @return 0 on success, -1 on error and errno is set appropriately.
		*/
		static int set_sock_send_timeo(int sockfd, int millisec);
		/**
		* @brief Set a timeout on receiving data. If you want to disable timeout, just simply
		*           call this function again with millisec set to be 0.
		*
		* @param sockfd socket descriptor to be set.
		* @param millisec timeout in milliseconds.
		*
		* @return 0 on success, -1 on error and errno is set appropriately.
		*/
		static int set_sock_rcv_timeo(int sockfd, int millisec);
		//************************************
		// Brief:	  Set the given fd recv buffer
		// Returns:   int (0 on success, -1 on error)
		// Parameter: int s (fd)
		// Parameter: uint32_t len (recv buffer len)
		//************************************
		static int set_recvbuf(int s, uint32_t len);
		static int set_sendbuf(int s, uint32_t len);
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

	class lib_tcp_cli_t : public lib_tcp_t
	{
	public:
		lib_tcp_cli_t(){}
		virtual ~lib_tcp_cli_t(){}
		/**
		* @brief Create a TCP connection
		*
		* @param const char* ipaddr,  the ip address to connect to.
		* @param in_port_t port,  the port to connect to.
		* @param int timeout,  abort the connecting attempt after timeout secs. If timeout is less than or equal to 0, 
		*                                then the connecting attempt will not be interrupted until error occurs.
		* @param int block,  true and the connected fd will be set blocking, false and the fd will be set nonblocking.
		*
		* @return int, the connected fd on success, -1 on error.
		*/
		// todo need test connect timeout [3/11/2012 meng]
		static int safe_tcp_connect(const char* ipaddr, in_port_t port, int timeout, bool block);
	protected:
		
	private:
		lib_tcp_cli_t(const lib_tcp_cli_t& cr);
		lib_tcp_cli_t& operator=(const lib_tcp_cli_t& cr);
	};

}//end namespace ice
