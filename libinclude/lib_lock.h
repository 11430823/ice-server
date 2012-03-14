/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	锁
	brief:		OK
*********************************************************************/
/*
pthread_mutex_t  互斥量的属性    快速互斥锁（ＰＴＨＲＥＡＤ＿ＭＵＴＥＸ＿ＩＮＩＴＩＡＬＩＺＥＲ） ->     多次进入一个临界互斥量    ？　　　（默认只能进入一次　　解锁一次　　．．．．如此循环）
　　递归互斥锁（ＰＴＨＲＥＡＤ＿ＲＥＣＵＲＳＩＶＥ＿ＭＵＴＥＸ＿ＩＮＩＴＩＡＬＩＺＥＲ＿ＮＰ）
  检错互斥锁（ＰＴＨＲＥＡＤ＿ＰＥＥＯＲＣＨＥＣＫ＿ＭＵＴＥＸ＿ＩＮＩＴＩＡＬＩＺＥＲ＿ＮＰ）
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
