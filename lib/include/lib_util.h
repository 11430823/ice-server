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

#ifdef  likely
#undef  likely
#endif
#define likely(x)  __builtin_expect(!!(x), 1)

#ifdef  unlikely
#undef  unlikely
#endif
#define unlikely(x)  __builtin_expect(!!(x), 0)

//////////////////////////////////////////////////////////////////////////
//使用宏管理成员变量
#define PROTECTED_READONLY_DEFAULT(varType, varName)\
	protected:	varType varName;\
	public:		varType get_##varName(void) { return varName; }

#define PRIVATE_READONLY_DEFAULT(varType, varName)\
	private:	varType varName;\
	public:		varType get_##varName(void) { return varName; }

#define PRIVATE_READONLY_BY_REF_DEFAULT(varType, varName)\
	private:	varType varName;\
	public:		const varType& get_##varName(void) { return varName; }

#define PRIVATE_RW_DEFAULT(varType, varName)\
	private:	varType varName;\
	public:		varType get_##varName(void) { return varName; } \
	public:		void set_##varName(varType var) { varName = var; }

#define PRIVATE_RW_BY_REF_DEFAULT(varType, varName)\
	private:	varType varName;\
	public:		const varType& get_##varName(void) { return varName; } \
	public:		void set_##varName(const varType& var) { varName = var; }



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

	#define FOREACH_PREV(container, it) \
		for(typeof((container).begin()) it = (container).end(); (it) != (container).begin(); --(it))

	//处理可被系统中断的函数返回EINTR时循环处理.(函数返回值必须为int)
	//example:int nRes = HANDLE_EINTR(::close(s));
	#define HANDLE_EINTR(x) ({\
		typeof(x) __eintr_code__;\
		do {\
			__eintr_code__ = x;\
		} while(unlikely(__eintr_code__ < 0 && EINTR == errno));\
		__eintr_code__;\
	})
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

#define safe_delete(p__){\
		delete (p__);\
		(p__) = NULL;\
	}

#define safe_delete_arr(p__){\
		delete [](p__);\
		(p__) = NULL;\
	}

#define safe_free(p__){\
		free (p__);\
		(p__) = NULL;\
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
				safe_delete(ptr);
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
				safe_delete(ptr.second);
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

}//end namespace ice
