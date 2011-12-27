/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		ok
*********************************************************************/

#pragma once
#include <stdint.h>
#include <iostream>
#include <sstream>
#include <string>

#ifdef  likely
#undef  likely
#endif
#define likely(x)  __builtin_expect(!!(x), 1)

#ifdef  unlikely
#undef  unlikely
#endif
#define unlikely(x)  __builtin_expect(!!(x), 0)

#define FOREACH(container,it) \
	for(typeof((container).begin()) it = (container).begin();(it)!=(container).end();++(it))

template <class T> 
void g_convert_from_string(T &value, const std::string &s) {
	std::stringstream ss(s);
	ss >> value;
}

template<typename T>
void g_safe_delete(T p)
{
	delete p;
	p = NULL;
};

template<typename T>
void g_safe_delete_arr(T p)
{
	delete []p;
	p = NULL;
};

/* *
 *  @brief 获取数组个数
 */
template<typename T>
uint32_t g_arr_num(const T& name)
{
	return sizeof(name)/sizeof(name[0]);
};

/* *
 *  @brief 切割字符(以单个符号为间断)like: 1,2,3 or 1;2;3 or aa/b/cc ...
 */
template <typename T>
void g_cat_string(std::vector<T>& dst_result, std::string& src_str, char tag)
{
	std::stringstream ss(src_str);        
	std::string sub_str;        
	while(std::getline(ss,sub_str,tag)){
		//以tag为间隔分割str的内容 
		T i;
		g_convert_from_string(i,sub_str);
		dst_result.push_back(i);
	}
};

struct DeletePtr
{
	template<typename T>
	void operator() (const T* ptr) const{
		if (ptr){
			g_safe_delete(ptr);
		}
	}
	//for_each(vs.begin(),vs.end(),DeletePtr());
};

struct DeletePair
{
	template<typename Ty1, typename Ty2>
	void operator() (const std::pair<Ty1, Ty2> &ptr) const{
		if (ptr.second){
			g_safe_delete(ptr.second);
		}		
	}
	//for_each(m_users.begin(), m_users.end(), DeletePair());
};