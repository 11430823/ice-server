/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	����
	brief:		ok
*********************************************************************/

#pragma once

#include "lib_include.h"

#ifndef likely
#define likely(x)  __builtin_expect(!!(x), 1)
#endif

#ifndef unlikely
#define unlikely(x)  __builtin_expect(!!(x), 0)
#endif


//////////////////////////////////////////////////////////////////////////
//ʹ�ú�����Ա����
#define PROTECTED_R(varType, varName)\
	protected:	varType varName;\
	public:		inline varType get_##varName(void){return this->varName;}

#define PROTECTED_RW(varType, varName)\
	protected:	varType varName;\
	public:		inline varType get_##varName(void){return this->varName;}\
	public:		inline void set_##varName(varType var){this->varName = var;}

#define PROTECTED_R_REF(varType, varName)\
	protected:	varType varName;\
	public:		inline const varType& get_##varName(void){return this->varName;}

#define PRIVATE_R(varType, varName)\
	private:	varType varName;\
	public:		inline varType get_##varName(void){return this->varName;}

#define PRIVATE(varType, varName)\
	private:	varType varName;\

#define PRIVATE_R_REF(varType, varName)\
	private:	varType varName;\
	public:		inline const varType& get_##varName(void){return this->varName;}

#define PRIVATE_RW(varType, varName)\
	private:	varType varName;\
	public:		inline varType get_##varName(void){return this->varName;} \
	public:		inline void set_##varName(varType var){this->varName = var;}

#define PRIVATE_RW_REF(varType, varName)\
	private:	varType varName;\
	public:		inline const varType& get_##varName(void){return this->varName;} \
	public:		inline void set_##varName(const varType& var){this->varName = var;}

#define PRIVATE_SATAIC_R(varType, varName)\
	private:	static varType varName;\
	public:		static inline varType get_##varName(void){return this->varName;}

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

#define SAFE_STRNCPY(dst, src) {\
	if (src){\
		::strncpy(dst, src, sizeof(dst) - 1); \
		dst[sizeof(dst) - 1] = '\0'; \
	}else{\
		dst[0] = '\0'; \
	}\
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
	#define FAIL -1

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
	// Parameter: std::vector<T> & result �и��Ľ��
	// Parameter: std::string & src	���и���ַ���
	// Parameter: char tag	�и����ݷ���
	//************************************
	template <typename T>
	inline void g_cat_string(std::vector<T>& result, const std::string& src, char tag){
		std::stringstream ss(src);        
		std::string sub_str;        
		while(std::getline(ss, sub_str, tag)){
			//��tagΪ����ָ�str������ 
			T t;
			convert_from_string(t, sub_str);
			result.push_back(t);
		}
	};

	/**
	* @brief	�ָ�(separator��ÿ���ַ���������Ϊ�ָ���)
	* @param	const std::string & src
	* @param	const std::string & separator
	* @param	std::vector<T> & dest
	Example

	std::string src = "1jf456j89";
	std::vector<std::string> v;
	split(src, "jf",v);

	1
	456
	89
	*/
	template <typename T>
	inline void split(const std::string& src, const std::string& separator, std::vector<T>& dest){
		std::string str = src;
		std::string substring;
		std::string::size_type start = 0, index;

		do{
			index = str.find_first_of(separator, start);
			if (index != std::string::npos){    
				substring = str.substr(start, index-start);

				T t;
				convert_from_string(t, substring);
				dest.push_back(t);

				start = str.find_first_not_of(separator, index);
				if (start == std::string::npos){
					return;
				}
			}
		}while(index != std::string::npos);

		//the last token
		substring = str.substr(start);

		T t;
		convert_from_string(t, substring);
		dest.push_back(t);
	}

	//ɾ��(�ǿ�)ָ�벢�ÿ�
	//ʹ�÷���://for_each(vector.begin(),vector.end(),lib_delete_ptr());
	struct lib_delete_ptr{
		template <typename T>
		void operator() (const T* ptr) const{
			if (ptr){
				SAFE_DELETE(ptr);
			}
		}
	};

	//ɾ��(�ǿ�)map��valλ���ϵ�ָ�벢�ÿ�
	//�÷�://for_each(map.begin(), map.end(), lib_delete_pair());
	struct lib_delete_pair{
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
	 * @brief	�Ƿ�С���ֽ���
	 * @return	bool
	 */
	static inline bool is_little_endian(){
		union ut{
			int16_t s;
			char c[2];
		};

		ut u;
		u.s = 0x0102;
		if(1 == u.c[0] && 2 == u.c[1]){
			return false;
		}
		return true;
	}

	/**
	* @brief	��װ����16����
	*/
	static inline void bin2hex(std::string& dst, char* src, uint32_t len){
		uint32_t hex;
		char c;
		for(uint32_t i=0;i<len;i++){
			hex=((unsigned char)src[i])>>4;
			c = (hex < 10) ? ('0'+hex) : ('A'- 10 + hex);
			dst += c;

			hex = ((unsigned char)src[i]) & 0x0F;
			c = (hex < 10) ? ('0' + hex) : ('A'- 10 + hex);
			dst += c;

			dst += ' ';
		}
	}

}//end namespace ice
