/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	锁
	brief:		OK
*********************************************************************/

#include <pthread.h>

#include "lib_util.h"

namespace ice{
	//互斥锁
	class lib_lock_mutex_t
	{
		PROPERTY_READONLY_DEFAULT(pthread_mutex_t, lock_mutex);
	public:
		lib_lock_mutex_t();
		virtual ~lib_lock_mutex_t();
	public:
		inline void lock(){
			::pthread_mutex_lock(&this->lock_mutex);
		}
		inline void ulock(){
			::pthread_mutex_unlock(&this->lock_mutex);
		}
	private:
		lib_lock_mutex_t(const lib_lock_mutex_t& cr);
		lib_lock_mutex_t& operator=(const lib_lock_mutex_t& cr);
	};

}//end namespace ice
