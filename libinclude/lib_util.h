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
#include <vector>

namespace ice {

#ifdef  likely
#undef  likely
#endif
#define likely(x)  __builtin_expect(!!(x), 1)

#ifdef  unlikely
#undef  unlikely
#endif
#define unlikely(x)  __builtin_expect(!!(x), 0)

#define FOREACH(container, it) \
	for(typeof((container).begin()) it = (container).begin(); (it) != (container).end(); ++(it))

#define FOREACH_PREV(container, it) \
	for(typeof((container).begin()) it = (container).end(); (it) != (container).begin(); --(it))
//************************************
// Brief:     ���ַ���ת��Ϊ��������
// Returns:   void
// Parameter: T & value	��������
// Parameter: const std::string & s	��ת�����ַ���
//************************************
template <class T> 
inline void convert_from_string(T &value, const std::string &s) {
	std::stringstream ss(s);
	ss >> value;
}

template<typename T>
inline void safe_delete(T p)
{
	delete p;
	p = NULL;
};

template<typename T>
inline void safe_delete_arr(T p)
{
	delete []p;
	p = NULL;
};

//************************************
// Brief:     ��������,��ȡ�������
// Returns:   uint32_t	�������
// Parameter: const T & name ��������
//************************************
template<typename T>
inline uint32_t get_arr_num(const T& name)
{
	return sizeof(name)/sizeof(name[0]);
};

//************************************
// Brief:     �и��ַ�(�Ե�������Ϊ���)like: 1,2,3 or 1;2;3 or aa/b/cc ...
// Returns:   void
// Parameter: std::vector<T> & dst_result �и��Ľ��
// Parameter: std::string & src_str	���и���ַ���
// Parameter: char tag	�и����ݷ���
//************************************
template <typename T>
inline void cat_string(std::vector<T>& dst_result, std::string& src_str, char tag)
{
	std::stringstream ss(src_str);        
	std::string sub_str;        
	while(std::getline(ss, sub_str, tag)){
		//��tagΪ����ָ�str������ 
		T i;
		convert_from_string(i,sub_str);
		dst_result.push_back(i);
	}
};

//ɾ��(�ǿ�)ָ�벢�ÿ�
//ʹ�÷���://for_each(vector.begin(),vector.end(),DeletePtr());
struct DeletePtr
{
	template<typename T>
	void operator() (const T* ptr) const{
		if (ptr){
			safe_delete(ptr);
		}
	}
};

//ɾ��(�ǿ�)map��valλ���ϵ�ָ�벢�ÿ�
//�÷�://for_each(map.begin(), map.end(), DeletePair());
struct DeletePair
{
	template<typename Ty1, typename Ty2>
	void operator() (const std::pair<Ty1, Ty2> &ptr) const{
		if (ptr.second){
			safe_delete(ptr.second);
		}		
	}
};

//////////////////////////////////////////////////////////////////////////
//ʹ�ú�����Ա����
#define PROPERTY_READONLY_DEFAULT(varType, varName)\
		private:	varType varName;\
		public:		varType get_##varName(void) { return varName; }

#define PROPERTY_READONLY_BY_REF_DEFAULT(varType, varName)\
		private:	varType varName;\
		public:		const varType& get_##varName(void) { return varName; }

#define PROPERTY_RW_DEFAULT(varType, varName)\
		private:	varType varName;\
		public:		varType get_##varName(void) { return varName; } \
		public:		void set_##varName(varType var) { varName = var; }

#define PROPERTY_RW_BY_REF_DEFAULT(varType, varName)\
		private:	varType varName;\
		public:		const varType& get_##varName(void) { return varName; } \
		public:		void set_##varName(const varType& var) { varName = var; }

}//end of namespace ice