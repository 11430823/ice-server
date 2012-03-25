/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	各种内存类型
	brief:		OK
*********************************************************************/

#pragma once

#include <stdint.h>

#include "lib_util.h"

namespace ice{
	//////////////////////////////////////////////////////////////////////////
	//自动无限放大,但是不会自动紧缩.
	class lib_active_buf_t
	{
		PROPERTY_READONLY_DEFAULT(char*, data);//数据头指针
		PROPERTY_READONLY_DEFAULT(uint32_t, total_len);//已分配的总长度
		PROPERTY_READONLY_DEFAULT(uint32_t, write_pos);//已使用到的位置
	public:
		lib_active_buf_t();
		virtual ~lib_active_buf_t();
		void push_back(const char* const pdata, uint32_t len);
		//************************************
		// Brief:	  从数据头中弹出长度为len的数据
		// Returns:   u32 (m_pos,剩余的数据长度(0:没有剩余))
		// Parameter: u32 len (弹出长度)
		//************************************
		uint32_t pop_front(uint32_t len);
		//************************************
		// Brief:	清理所有数据  
		// Returns:   void ()
		//************************************
		inline void clean();
	private:
		inline void init_data();
	private:
		lib_active_buf_t(const lib_active_buf_t& cr);
		lib_active_buf_t& operator=(const lib_active_buf_t& cr);
	};

}//end namespace ice