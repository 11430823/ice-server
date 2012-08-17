/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	OK
	brief:		网络
*********************************************************************/

#pragma once

#include "lib_util.h"
#include "lib_file.h"

namespace ice{
	class lib_net_t{
		PROTECTED_RW(int, fd);
	public:
		lib_net_t(){
			this->fd = INVALID_FD;
		}
		virtual ~lib_net_t(){
			lib_file_t::close_fd(this->fd);
		}
	public:
		/**
		* @brief Send `total` bytes of data
		* @param const void* data,  data to be sent.
		* @param int len,  number of bytes in len to be sent.
		* @return int, number of bytes sent on success, -1 on error.
		*/
		virtual int send(const void* data, int len) = 0;
		/**
		* @brief Receive data  
		* @param const void* data,  buffer to hold the receiving data.
		* @param int len,  size of `buf`.
		* @return int, number of bytes receive on success, -1 on error (没有处理EAGAIN), 0 on connection closed by peer.
		*/
		virtual int recv(void* data, int len) = 0;
	protected:
	private:
		lib_net_t(const lib_net_t& cr);
		lib_net_t& operator=(const lib_net_t& cr);
	};
}//end namespace ice