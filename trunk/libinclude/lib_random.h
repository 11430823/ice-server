/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	随机数
	brief:		
*********************************************************************/

#pragma once

namespace ice{

	/**
	* @brief 产生给定范围内的随机数。如果需要更加随机，可以在程序起来的时候调用一次srand()。
	* @param min 随机出来的最小数。
	* @param max 随机出来的最大数。
	* @return min和max之间的随机数，包括min和max。//return [min,max]
	*/
	int random(int min, int max);

	/**
	* @brief 根据输入的字符串，生成一个32位的哈希值
	* @param p 需要生成哈希值的字符串
	* @return 字符串对应的哈希值
	*/
	int hash(const char* p);







}//end namespace ice
