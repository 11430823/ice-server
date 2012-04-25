/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	ok
	brief:		网络
*********************************************************************/

#pragma once

#include "lib_util.h"

namespace ice{
	class lib_net_t
	{
		PROTECTED_RW_DEFAULT(int, fd);
	public:
		lib_net_t();
		virtual ~lib_net_t();
	public:
		/**
		* @brief Send `total` bytes of data
		* @param const void* buf,  data to be sent.
		* @param int total,  number of bytes in total to be sent.
		* @return int, number of bytes sent on success, -1 on error.
		*/
		virtual int send(const void* buf, int total) = 0;
		/**
		* @brief Receive data  
		* @param const void* buf,  buffer to hold the receiving data.
		* @param int bufsize,  size of `buf`.
		* @return int, number of bytes receive on success, -1 on error (没有处理EAGAIN), 0 on connection closed by peer.
		*/
		virtual int recv(void* buf, int bufsize) = 0;

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

		static uint32_t ip2int(const char* ip);
		std::string ip2str(uint32_t ip);
	protected:
	private:
		lib_net_t(const lib_net_t& cr);
		lib_net_t& operator=(const lib_net_t& cr);
	};
}//end namespace ice