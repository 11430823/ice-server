/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	ʱ�亯��
	brief:		
*********************************************************************/

#pragma once

#include <time.h>

namespace ice{
	class lib_time
	{
	public:
		lib_time(){}
		//virtual ~lib_time(){}
		static time_t get_now_second();
		/**
		* @brief ��`tm_cur`��Сʱȡ����ʱ�䡣����`tm_cur`��ʱ����20081216-16:10:25���򷵻ص�ʱ����20081216-16:00:00��
		*
		* @param struct tm tm_cur,  ��Ҫ��Сʱȡ�����ʱ�䡣
		*
		* @return time_t, ��Сʱȡ������Epoch��ʼ��������
		*/
		static time_t get_integral_tm_hour(tm* tm_cur);
		//get zodiac index
		static int get_zodiac( int mon, int mon_day );
	protected:
		
	private:
		lib_time(const lib_time& cr);
		lib_time& operator=(const lib_time& cr);
	};
}//end namespace ice
