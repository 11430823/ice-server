#include "lib_time.h"

time_t ice::lib_time_t::get_integral_tm_hour( tm* tm_cur )
{
	tm_cur->tm_min  = 0;
	tm_cur->tm_sec  = 0;

	return mktime(tm_cur);
}

int ice::lib_time_t::get_zodiac( int mon, int mon_day )
{
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
