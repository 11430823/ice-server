/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	��
	brief:		OK
*********************************************************************/
/*
pthread_mutex_t  ������������    ���ٻ��������Уԣȣңţ��ģߣͣգԣţأߣɣΣɣԣɣ��̣ɣڣţң� ->     ��ν���һ���ٽ绥����    ����������Ĭ��ֻ�ܽ���һ�Ρ�������һ�Ρ��������������ѭ����
�����ݹ黥�������Уԣȣңţ��ģߣңţãգңӣɣ֣ţߣͣգԣţأߣɣΣɣԣɣ��̣ɣڣţңߣΣУ�
  ����������Уԣȣңţ��ģߣУţţϣңãȣţãˣߣͣգԣţأߣɣΣɣԣɣ��̣ɣڣţңߣΣУ�
  */
#include <pthread.h>

#include "lib_util.h"

namespace ice{

	class lib_lock_mutex
	{
	public:
		lib_lock_mutex();
		virtual ~lib_lock_mutex();
	public:
		inline void lock();
		inline void ulock();
		PROPERTY_READONLY_DEFAULT(pthread_mutex_t, lock_mutex);
	private:
		lib_lock_mutex(const lib_lock_mutex& cr);
		lib_lock_mutex& operator=(const lib_lock_mutex& cr);
	};

}//end namespace ice
