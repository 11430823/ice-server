/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	用于进行字节序转换。如果原本是网络序，则转成本机序；如果原本是本机序，则转成网络序.
				用法：val = bswap(val);。可以对任意大小的整数类型进行字节序转换。
	brief:		ok
*********************************************************************/

#pragma once

#include "lib_include.h"

namespace ice{

	class lib_byte_swap_t{
	public:
		static inline int8_t bswap(int8_t x){
			return x;
		}
		static inline uint8_t bswap(uint8_t x){
			return x;
		}
		static inline int16_t bswap(int16_t x){
			return ::bswap_16(x);
		}
		static inline uint16_t bswap(uint16_t x){
			return ::bswap_16(x);
		}
		static inline int32_t bswap(int32_t x){
			return ::bswap_32(x);
		}
		static inline uint32_t bswap(uint32_t x){
			return ::bswap_32(x);
		}
		static inline int64_t bswap(int64_t x){
			return ::bswap_64(x);
		}
		static inline uint64_t bswap(uint64_t x){
			return ::bswap_64(x);
		}
#if __WORDSIZE == 32
		static inline long bswap(long x){
			return ::bswap_32(x);
		}
		static inline unsigned long bswap(unsigned long x){
			return ::bswap_32(x);
		}
#endif
	protected:
	private:
		lib_byte_swap_t(const lib_byte_swap_t& cr);
		lib_byte_swap_t& operator=(const lib_byte_swap_t& cr);
	};

}//end namespace ice
