/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	ok
	brief:		ÍøÂç
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
		* @return int, number of bytes receive on success, -1 on error, 0 on connection closed by peer.
		*/
		virtual int recv(void* buf, int bufsize) = 0;
	protected:
	private:
		lib_net_t(const lib_net_t& cr);
		lib_net_t& operator=(const lib_net_t& cr);
	};
}//end namespace ice