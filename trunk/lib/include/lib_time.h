/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	时间函数
	brief:		
*********************************************************************/

#pragma once

#include <time.h>

namespace ice{
	class lib_time
	{
	public:
		//************************************
		// Brief:     returns the time as the number of seconds since the Epoch, 1970-01-01 00:00:00 +0000 (UTC).
		// Returns:   time_t
		//************************************
		static inline time_t get_now_second();

	protected:
		
	private:
		lib_time(const lib_time& cr);
		lib_time& operator=(const lib_time& cr);
	private://TODO
		/**
		* @brief 把`tm_cur`按小时取整点时间。比如`tm_cur`的时间是20081216-16:10:25，则返回的时间是20081216-16:00:00。
		*
		* @param struct tm tm_cur,  需要按小时取整点的时间。
		*
		* @return time_t, 按小时取整点后从Epoch开始的秒数。
		*/
		static inline time_t get_integral_tm_hour(tm* tm_cur);
		//get zodiac index
		static inline int get_zodiac( int mon, int mon_day );
	};
}//end namespace ice
