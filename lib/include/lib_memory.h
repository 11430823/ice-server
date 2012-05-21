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
		PRIVATE_R(char*, data);//数据头指针
		PRIVATE_R(uint32_t, total_len);//已分配的总长度
		PRIVATE_R(uint32_t, write_pos);//已使用到的位置
	public:
		lib_active_buf_t();
		virtual ~lib_active_buf_t();
		//************************************
		// Brief:     向后插入数据
		// Returns:   void
		// Parameter: const char * const pdata 数据
		// Parameter: uint32_t len 数据长度
		//************************************
		void push_back(const char* const pdata, uint32_t len);
		//************************************
		// Brief:	  从数据头中弹出长度为len的数据
		// Returns:   u32 (m_pos,剩余的数据长度(0:没有剩余))
		// Parameter: u32 len (弹出长度)
		//************************************
		uint32_t pop_front(uint32_t len);
		//************************************
		// Brief:	清理所有数据/释放内存空间  
		// Returns:   void ()
		//************************************
		void clean(){
			SAFE_FREE(this->data);
			this->init_data();
		}
	private:
		void init_data(){
			this->data = NULL;
			this->total_len = 0;
			this->write_pos = 0;
		}
	private:
		lib_active_buf_t(const lib_active_buf_t& cr);
		lib_active_buf_t& operator=(const lib_active_buf_t& cr);
	};

}//end namespace ice