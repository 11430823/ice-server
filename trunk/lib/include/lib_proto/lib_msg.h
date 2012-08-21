/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	协议消息
	brief:		
*********************************************************************/

#pragma once

namespace ice{
	class lib_msg_byte_t
	{
	public:
		lib_msg_byte_t(){}
		virtual ~lib_msg_byte_t(){}
	protected:
		
	private:
		lib_msg_byte_t(const lib_msg_byte_t& cr);
		lib_msg_byte_t& operator=(const lib_msg_byte_t& cr);
	};
	
	class lib_msg_t
	{
	public:
		lib_msg_t(){}
		virtual ~lib_msg_t(){}
		virtual bool read(lib_msg_byte_t& msg_byte){return true;}
		virtual bool write(lib_msg_byte_t& msg_byte){return true;}
	protected:
		
	private:
		lib_msg_t(const lib_msg_t& cr);
		lib_msg_t& operator=(const lib_msg_t& cr);
	};
	
}//end namespace ice  



