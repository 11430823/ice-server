/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	杂项
	brief:		ok
*********************************************************************/

#pragma once

#include <stdint.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <errno.h>
#include <stdlib.h>

#ifndef likely
#define likely(x)  __builtin_expect(!!(x), 1)
#endif

#ifndef unlikely
#define unlikely(x)  __builtin_expect(!!(x), 0)
#endif


//////////////////////////////////////////////////////////////////////////
//使用宏管理成员变量
#define PROTECTED_R_DEFAULT(varType, varName)\
	protected:	varType varName;\
	public:		varType get_##varName(void) { return this->varName; }

#define PROTECTED_RW_DEFAULT(varType, varName)\
	protected:	varType varName;\
	public:		varType get_##varName(void) { return this->varName; } \
	public:		void set_##varName(varType var) { this->varName = var; }

#define PRIVATE_R_DEFAULT(varType, varName)\
	private:	varType varName;\
	public:		varType get_##varName(void) { return this->varName; }

#define PRIVATE_R_BY_REF_DEFAULT(varType, varName)\
	private:	varType varName;\
	public:		const varType& get_##varName(void) { return this->varName; }

#define PRIVATE_RW_DEFAULT(varType, varName)\
	private:	varType varName;\
	public:		varType get_##varName(void) { return this->varName; } \
	public:		void set_##varName(varType var) { this->varName = var; }

#define PRIVATE_RW_BY_REF_DEFAULT(varType, varName)\
	private:	varType varName;\
	public:		const varType& get_##varName(void) { return this->varName; } \
	public:		void set_##varName(const varType& var) { this->varName = var; }

#define SAFE_DELETE(p__){\
		delete p__;\
		p__ = NULL;\
	}

#define SAFE_DELETE_ARR(p__){\
		delete [](p__);\
		(p__) = NULL;\
	}

#define SAFE_FREE(p__){\
		free (p__);\
		(p__) = NULL;\
	}

//处理可被系统中断的函数返回EINTR时循环处理.(函数返回值必须为int)
//example:int nRes = HANDLE_EINTR(::close(s));
#define HANDLE_EINTR(x) ({\
	typeof(x) __eintr_code__;\
	do {\
	__eintr_code__ = x;\
	} while(unlikely(__eintr_code__ < 0 && EINTR == errno));\
	__eintr_code__;\
	})

namespace ice{
	#define SUCC 0
	#define ERR  -1

		//协助
	#define NOTE//需要注意
	#define TODO//需要完成
	#define FOR_DEBUG//为了调试
	#define OUT//输出参数
	#define IN//输入参数
	//判断
	#define	IS_NULL(p_)	(NULL == (p_))
	#define	NO_NULL(p_)	(NULL != (p_))
	#define	IS_ZERO(n_)	(0 == (n_))
	#define	NO_ZERO(n_)	(0 != (n_))
	#define IS_SUCC(n_) (SUCC == (n_))
	#define IS_ERR(n_) (SUCC != (n_))

	#define FOREACH(container, it) \
		for(typeof((container).begin()) it = (container).begin(); (it) != (container).end(); ++(it))

	//靠
	#define FOREACH_REVERSE(cotainer, it) \
		for(typeof((container).rbegin()) it = (container).rbegin(); (it) != (container).rend(); ++(it))


	//************************************
	// Brief:     由字符串转换为所需类型
	// Returns:   void
	// Parameter: T & value	所需类型
	// Parameter: const std::string & s	待转换的字符串
	// notice: Parameter: const std::string & s
	// 1.(中间有空格的话,它会用空格分割,只取空格前的部分)
	// 2. 如果你习惯了C分割的atoi直接将空字符串转成0，那么用stringstream时，千万不要做这种事情
	//convert_from_string(x, "");
	//x输出不一定为0
	//************************************
	template <typename T> 
	inline void convert_from_string(T &value, const std::string &s) {
		std::stringstream ss(s);
		ss >> value;
	}

	//************************************
	// Brief:     由数组名,获取数组个数
	// Returns:   uint32_t	数组个数
	// Parameter: const T & name 数组名称
	//************************************
	template <typename T>
	inline uint32_t get_arr_num(const T& name){
		return sizeof(name)/sizeof(name[0]);
	};

	//************************************
	// Brief:     切割字符(以单个符号为间断)like: 1,2,3 or 1;2;3 or aa/b/cc ...
	// Returns:   void
	// Parameter: std::vector<T> & dst_result 切割后的结果
	// Parameter: std::string & src_str	待切割的字符串
	// Parameter: char tag	切割依据符号
	//************************************
	template <typename T>
	inline void cat_string(std::vector<T>& dst_result, std::string& src_str, char tag){
		std::stringstream ss(src_str);        
		std::string sub_str;        
		while(std::getline(ss, sub_str, tag)){
			//以tag为间隔分割str的内容 
			T i;
			convert_from_string(i,sub_str);
			dst_result.push_back(i);
		}
	};

	//删除(非空)指针并置空
	//使用方法://for_each(vector.begin(),vector.end(),lib_delete_ptr());
	struct lib_delete_ptr
	{
		template <typename T>
		void operator() (const T* ptr) const{
			if (ptr){
				SAFE_DELETE(ptr);
			}
		}
	};

	//删除(非空)map中val位置上的指针并置空
	//用法://for_each(map.begin(), map.end(), lib_delete_pair());
	struct lib_delete_pair
	{
		template <typename Ty1, typename Ty2>
		void operator() (const std::pair<Ty1, Ty2> &ptr) const{
			if (ptr.second){
				SAFE_DELETE(ptr.second);
			}		
		}
	};

	//sort,降序(大->小)
	struct desc_sort{
		uint32_t level;
		bool operator < (const desc_sort& r) const {
			return level > r.level;
		}
	};

	//////////////////////////////////////////////////////////////////////////
	//不可拷贝,基类
	class lib_non_copyable{
	protected:
		lib_non_copyable() {}
	private:
		lib_non_copyable( const lib_non_copyable& );
		const lib_non_copyable& operator=( const lib_non_copyable& );
	};

	/**
	 * @brief 根据输入的字符串，生成一个32位的哈希值
	 * @param p 需要生成哈希值的字符串
	 * @return 字符串对应的哈希值
	 */
	static inline uint32_t gen_u32_hash(const char* p)
	{
		uint32_t h = 0;
		while (*p) {
			h = h * 11 + (*p << 4) + (*p >> 4);
			p++;
		}
		return h;
	}

}//end namespace ice
