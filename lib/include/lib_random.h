/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	随机数
	brief:		OK
*********************************************************************/

#pragma once

namespace ice{

	class lib_random
	{
	public:
		/**
		* @brief 产生给定范围内的随机数。如果需要更加随机，可以在程序起来的时候调用一次srand()。
		* @param min 随机出来的最小数。
		* @param max 随机出来的最大数。
		* @return min和max之间的随机数，包括min和max。//return [min,max]
		*/
		static inline int random(int min, int max);
		/**
		* @brief 根据输入的字符串，生成一个32位的哈希值
		* @param p 需要生成哈希值的字符串
		* @return 字符串对应的哈希值
		*/
		static inline int hash(const char* p);
	protected:
		
	private:
		lib_random(const lib_random& cr);
		lib_random& operator=(const lib_random& cr);
	};

}//end namespace ice
