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

		virtual int recv(void* buf, int bufsize);

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
