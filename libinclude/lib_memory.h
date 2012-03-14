/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	�����ڴ�����
	brief:		
*********************************************************************/

#pragma once

#include <stdint.h>

#include <lib_util.h>

namespace ice{
	class lib_memory_active_buf
	{
	public:
		lib_memory_active_buf();
		virtual ~lib_memory_active_buf();
		void push_back(const char* const pdata, uint32_t len);
		//************************************
		// Brief:	  ������ͷ�е�������Ϊlen������
		// Returns:   u32 (m_pos,ʣ������ݳ���(0:û��ʣ��))
		// Parameter: u32 len (��������)
		//************************************
		uint32_t pop_front(uint32_t len);
		//************************************
		// Brief:	������������  
		// Returns:   void ()
		//************************************
		void clean();
	private:
		void init_data();
		PROPERTY_READONLY_DEFAULT(char*, data);//����ͷָ��
		PROPERTY_READONLY_DEFAULT(uint32_t, total_len);//�ѷ�����ܳ���
		PROPERTY_READONLY_DEFAULT(uint32_t, write_pos);//��ʹ�õ���λ��
	private:
		lib_memory_active_buf(const lib_memory_active_buf& cr);
		lib_memory_active_buf& operator=(const lib_memory_active_buf& cr);
	};

}//end namespace ice