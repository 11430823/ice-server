/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	����
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
//ʹ�ú�����Ա����
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

//����ɱ�ϵͳ�жϵĺ�������EINTRʱѭ������.(��������ֵ����Ϊint)
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

		//Э��
	#define NOTE//��Ҫע��
	#define TODO//��Ҫ���
	#define FOR_DEBUG//Ϊ�˵���
	#define OUT//�������
	#define IN//�������
	//�ж�
	#define	IS_NULL(p_)	(NULL == (p_))
	#define	NO_NULL(p_)	(NULL != (p_))
	#define	IS_ZERO(n_)	(0 == (n_))
	#define	NO_ZERO(n_)	(0 != (n_))
	#define IS_SUCC(n_) (SUCC == (n_))
	#define IS_ERR(n_) (SUCC != (n_))

	#define FOREACH(container, it) \
		for(typeof((container).begin()) it = (container).begin(); (it) != (container).end(); ++(it))

	//��
	#define FOREACH_REVERSE(cotainer, it) \
		for(typeof((container).rbegin()) it = (container).rbegin(); (it) != (container).rend(); ++(it))


	//************************************
	// Brief:     ���ַ���ת��Ϊ��������
	// Returns:   void
	// Parameter: T & value	��������
	// Parameter: const std::string & s	��ת�����ַ���
	// notice: Parameter: const std::string & s
	// 1.(�м��пո�Ļ�,�����ÿո�ָ�,ֻȡ�ո�ǰ�Ĳ���)
	// 2. �����ϰ����C�ָ��atoiֱ�ӽ����ַ���ת��0����ô��stringstreamʱ��ǧ��Ҫ����������
	//convert_from_string(x, "");
	//x�����һ��Ϊ0
	//************************************
	template <typename T> 
	inline void convert_from_string(T &value, const std::string &s) {
		std::stringstream ss(s);
		ss >> value;
	}

	//************************************
	// Brief:     ��������,��ȡ�������
	// Returns:   uint32_t	�������
	// Parameter: const T & name ��������
	//************************************
	template <typename T>
	inline uint32_t get_arr_num(const T& name){
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
	inline void cat_string(std::vector<T>& dst_result, std::string& src_str, char tag){
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
	//ʹ�÷���://for_each(vector.begin(),vector.end(),lib_delete_ptr());
	struct lib_delete_ptr
	{
		template <typename T>
		void operator() (const T* ptr) const{
			if (ptr){
				SAFE_DELETE(ptr);
			}
		}
	};

	//ɾ��(�ǿ�)map��valλ���ϵ�ָ�벢�ÿ�
	//�÷�://for_each(map.begin(), map.end(), lib_delete_pair());
	struct lib_delete_pair
	{
		template <typename Ty1, typename Ty2>
		void operator() (const std::pair<Ty1, Ty2> &ptr) const{
			if (ptr.second){
				SAFE_DELETE(ptr.second);
			}		
		}
	};

	//sort,����(��->С)
	struct desc_sort{
		uint32_t level;
		bool operator < (const desc_sort& r) const {
			return level > r.level;
		}
	};

	//////////////////////////////////////////////////////////////////////////
	//���ɿ���,����
	class lib_non_copyable{
	protected:
		lib_non_copyable() {}
	private:
		lib_non_copyable( const lib_non_copyable& );
		const lib_non_copyable& operator=( const lib_non_copyable& );
	};

	/**
	 * @brief ����������ַ���������һ��32λ�Ĺ�ϣֵ
	 * @param p ��Ҫ���ɹ�ϣֵ���ַ���
	 * @return �ַ�����Ӧ�Ĺ�ϣֵ
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
