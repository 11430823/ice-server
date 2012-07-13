/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	时间函数
	brief:		ok
*********************************************************************/

#pragma once

#include "lib_include.h"

namespace ice{
	class lib_time_t
	{
	public:
		//************************************
		// Brief:     returns the time as the number of seconds since the Epoch, 1970-01-01 00:00:00 +0000 (UTC).
		// Returns:   time_t
		//************************************
		static inline time_t get_now_second(){
			return ::time(NULL);
		}
		static inline void get_now_tm(struct tm& rtm){
			time_t now = get_now_second();
			::localtime_r(&now, &rtm);
		}

		/**
		* @brief 把`tm_cur`按小时取整点时间。比如`tm_cur`的时间是20081216-16:10:25，则返回的时间是20081216-16:00:00。
		*
		* @param struct tm tm_cur,  需要按小时取整点的时间。
		*
		* @return time_t, 按小时取整点后从Epoch开始的秒数。
		*/
		static inline time_t get_integral_tm_hour(tm* tm_cur){
			tm_cur->tm_min  = 0;
			tm_cur->tm_sec  = 0;

			return mktime(tm_cur);
		}
		//get zodiac index
		static inline int get_zodiac( int mon, int mon_day ){
			int zodiac = 0;
			static const int zodiac_day[12][2] = {
				{1,20},//2
				{2,19},//3
				{3,21},//4
				{4,21},//5
				{5,21},//6
				{6,22},//7
				{7,23},//8
				{8,23},//9
				{9,23},//10
				{10,23},//11
				{11,22},//12
				{12,22}//1
			};
			for(int i = 0; i<12 ;i++){
				if(mon == zodiac_day[i][0]){
					if(mon_day >= zodiac_day[i][1]){
						zodiac = (i + 2) % 12;
						break;
					}else{
						i == 0 ? zodiac = 1 : zodiac = i + 1;
						break;
					}
				}
			}
			return zodiac;
		}
	protected:
	private:
		lib_time_t(const lib_time_t& cr);
		lib_time_t& operator=(const lib_time_t& cr);
	};
}//end namespace ice
