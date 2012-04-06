/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	�����ڴ�����
	brief:		OK
*********************************************************************/

#pragma once

#include <stdint.h>

#include "lib_util.h"

namespace ice{
	//////////////////////////////////////////////////////////////////////////
	//�Զ����޷Ŵ�,���ǲ����Զ�����.
	class lib_active_buf_t
	{
		PRIVATE_READONLY_DEFAULT(char*, data);//����ͷָ��
		PRIVATE_READONLY_DEFAULT(uint32_t, total_len);//�ѷ�����ܳ���
		PRIVATE_READONLY_DEFAULT(uint32_t, write_pos);//��ʹ�õ���λ��
	public:
		lib_active_buf_t();
		virtual ~lib_active_buf_t();
		//************************************
		// Brief:     ����������
		// Returns:   void
		// Parameter: const char * const pdata ����
		// Parameter: uint32_t len ���ݳ���
		//************************************
		void push_back(const char* const pdata, uint32_t len);
		//************************************
		// Brief:	  ������ͷ�е�������Ϊlen������
		// Returns:   u32 (m_pos,ʣ������ݳ���(0:û��ʣ��))
		// Parameter: u32 len (��������)
		//************************************
		uint32_t pop_front(uint32_t len);
		//************************************
		// Brief:	������������/�ͷ��ڴ�ռ�  
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