/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	协议消息
	brief:
	读:
		单:
			1.纯系统字段
			2.纯结构体字段.OK
		list:
			1.系统字段 OK
			2.结构体字段
*********************************************************************/

#pragma once

#include "lib_util.h"
#include "lib_packer.h"

#define  P_IN dynamic_cast<typeof p_in>(c_in);
#define  P_OUT dynamic_cast<typeof p_out>(c_out);

namespace ice{

#ifdef ICE_DEF_BIG_ENDIAN
#define GEN_READ_SINGLE_VALUE_FUNC(func, value_type) \
	inline bool func(value_type& value){\
		if (!this->is_read_only) return false;\
		if(this->postion+sizeof(value_type)<=this->size) {\
			value = *(value_type*)(this->buf+this->postion);\
			value = lib_byte_swap_t::bswap(value);\
			this->postion += sizeof(value_type);\
			return true;\
		}\
		return false;\
	}
#else
#define GEN_READ_SINGLE_VALUE_FUNC(func, value_type) \
	inline bool func(value_type& value){\
		if (!this->is_read_only) return false;\
		if(this->postion+sizeof(value_type)<=this->size) {\
			value = *(value_type*)(this->buf+this->postion);\
			this->postion += sizeof(value_type);\
			return true;\
		}\
		return false;\
	}
#endif

	class lib_msg_byte_t
	{
		PRIVATE_RW(char*, buf);
		PRIVATE_RW(uint32_t, size);
		PRIVATE_RW(uint32_t, postion);
		PRIVATE_RW(bool, is_read_only);
	public:
		lib_msg_byte_t(char* data, uint32_t len){
			this->is_read_only = true;
			this->size = len;
			this->postion = 0;
			this->buf = data;
		}
		virtual ~lib_msg_byte_t(){}
		inline bool is_end(){return this->postion == this->size;}
		GEN_READ_SINGLE_VALUE_FUNC(read_uint32, uint32_t)
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
		virtual void init(){}
		virtual bool read(lib_msg_byte_t& msg_byte){return true;}
		virtual bool write(lib_msg_byte_t& msg_byte){return true;}
		
	protected:
		
	private:
		lib_msg_t(const lib_msg_t& cr);
		lib_msg_t& operator=(const lib_msg_t& cr);
	};
	
}//end namespace ice  



