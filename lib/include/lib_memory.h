/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	各种内存类型
	brief:		OK
*********************************************************************/

#pragma once

#include "lib_include.h"
#include "lib_util.h"

namespace ice{
	//////////////////////////////////////////////////////////////////////////
	//自动无限放大,但是不会自动紧缩.
	class lib_active_buf_t{
		PRIVATE_R(char*, data);//数据头指针
		PRIVATE_R(uint32_t, total_len);//已分配的总长度
		PRIVATE_R(uint32_t, write_pos);//已使用到的位置
	public:
		lib_active_buf_t(){
			this->init_data();
		}
		virtual ~lib_active_buf_t(){
			this->clean();
		}
		//************************************
		// Brief:     向后插入数据
		// Returns:   void
		// Parameter: const char * const pdata 数据
		// Parameter: uint32_t len 数据长度
		//************************************
		inline void push_back(const char* const pdata, uint32_t len){
			uint32_t remain_len = this->total_len - this->write_pos;
			if(remain_len >= len){
				memcpy(this->data + this->write_pos, pdata, len);
				this->write_pos += len;
			}else{
				if(0 == this->total_len){
					this->data = (char*)malloc(len);
					memcpy(this->data, pdata, len);
					this->total_len = len;
					this->write_pos = len;
				}else{
					this->data = (char*)realloc(this->data, len + this->write_pos);
					memcpy(this->data + this->write_pos, pdata, len);
					this->total_len = len + this->write_pos;
					this->write_pos = this->total_len;
				}
			}
		}
		//************************************
		// Brief:	  从数据头中弹出长度为len的数据
		// Returns:   u32 (m_pos,剩余的数据长度(0:没有剩余))
		// Parameter: u32 len (弹出长度)
		//************************************
		inline uint32_t pop_front(uint32_t len){
			if(this->write_pos <= len){
				//全部弹出
				this->clean();
			}else{
				::memmove(this->data, this->data + len, this->write_pos - len);
				this->write_pos -= len;
			}
			return this->write_pos;
		}
		//************************************
		// Brief:	清理所有数据/释放内存空间  
		// Returns:   void ()
		//************************************
		inline void clean(){
			SAFE_FREE(this->data);
			this->init_data();
		}
	private:
		inline void init_data(){
			this->data = NULL;
			this->total_len = 0;
			this->write_pos = 0;
		}
	private:
		lib_active_buf_t(const lib_active_buf_t& cr);
		lib_active_buf_t& operator=(const lib_active_buf_t& cr);
	};

}//end namespace ice