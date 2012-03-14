/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	用于对数据包进行解包，或者把数据打包成数据包
	brief:		ok
*********************************************************************/
#pragma once

#include <cstring>

#include "lib_byte_swap.h"

namespace ice{

	class lib_packer_t
	{
	public:
		/**
		* @brief 把val转换字节序，并且打包到(uint8_t*)pkg + idx，然后增加idx的值，idx的增量为val变量占用的字节数。\n
		*        比如val的类型是uint32_t，则idx的值增加4。
		* @param pkg 用于保存数据包的内存块。
		* @param val 需要打包到pkg里的值。
		* @param idx 指示val应该被打包到pkg中偏移量为idx的内存中。
		* @return void
		*/
		template <typename T>
		static inline void pack(void* pkg, T val, int& idx){
			*(reinterpret_cast<T*>(reinterpret_cast<uint8_t*>(pkg) + idx)) = lib_byte_swap_t::bswap(val);
			idx += sizeof val;
		}

		/**
		* @brief 把内存块val打包到(uint8_t*)pkg + idx，然后增加idx的值，idx的增量为len。
		* @param pkg 用于保存数据包的内存块。
		* @param val 需要打包到pkg里的内存块。
		* @param len 内存块val的大小（byte）。
		* @param idx 指示val应该被打包到pkg中偏移量为idx的内存中。
		* @return void
		*/
		static inline void pack(void* pkg, const void* val, std::size_t len, int& idx){
			memcpy(reinterpret_cast<uint8_t*>(pkg) + idx, val, len);
			idx += len;
		}

		/**
		* @brief 不转换val的字节序，并且打包到(uint8_t*)pkg + idx，然后增加idx的值，idx的增量为val变量占用的字节数。\n
		*        比如val的类型是uint32_t，则idx的值增加4。h == host。
		* @param pkg 用于保存数据包的内存块。
		* @param val 需要打包到pkg里的值。
		* @param idx 指示val应该被打包到pkg中偏移量为idx的内存中。
		* @return void
		*/
		template <typename T>
		static inline void pack_h(void* pkg, T val, int& idx){
			*(reinterpret_cast<T*>(reinterpret_cast<uint8_t*>(pkg) + idx)) = val;
			idx += sizeof val;
		}

		/**
		 * @brief 将(uint8_t*)pkg + idx解包到val，并且转换字节序，然后增加idx的值，idx的增量为val变量占用的字节数。\n
		 *        比如val的类型是uint32_t，则idx的值增加4。
		 * @param pkg 需要解包的内存块。
		 * @param val 解包后的数据保存到val里。
		 * @param idx 指示应该把pkg中偏移量从idx开始的数据解包到val中。
		 * @return void
		 */
		template <typename T>
		static inline void unpack(const void* pkg, T& val, int& idx){
			val = lib_byte_swap_t::bswap(*(reinterpret_cast<const T*>(reinterpret_cast<const uint8_t*>(pkg) + idx)));
			idx += sizeof val;
		}

		/**
		 * @brief 将(uint8_t*)pkg + idx解包到val，然后增加idx的值，idx的增量为len。
		 * @param pkg 需要解包的内存块。
		 * @param val 解包后的数据保存到内存块val里。
		 * @param len 内存块val的大小（byte）。
		 * @param idx 指示应该把pkg中偏移量从idx开始的数据解包到val中。
		 * @return void
		 */
		static inline void unpack(const void* pkg, void* val, std::size_t len, int& idx){
			memcpy(val, reinterpret_cast<const uint8_t*>(pkg) + idx, len);
			idx += len;
		}

		/**
		 * @brief 将(uint8_t*)pkg + idx解包到val，但不转换字节序，然后增加idx的值，idx的增量为val变量占用的字节数。\n
		 *        比如val的类型是uint32_t，则idx的值增加4。
		 * @param pkg 需要解包的内存块。
		 * @param val 解包后的数据保存到val里。
		 * @param idx 指示应该把pkg中偏移量从idx开始的数据解包到val中。
		 * @return void
		 */
		template <typename T>
		static inline void unpack_h(const void* pkg, T& val, int& idx){
			val = *(reinterpret_cast<const T*>(reinterpret_cast<const uint8_t*>(pkg) + idx));
			idx += sizeof val;
		}
	protected:
		
	private:
		lib_packer_t(const lib_packer_t& cr);
		lib_packer_t& operator=(const lib_packer_t& cr);
	};

}//end namespace ice


