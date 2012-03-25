#include "lib_lock.h"

ice::lib_lock_mutex_t::lib_lock_mutex_t()
{
	::pthread_mutex_init(&this->lock_mutex,NULL);
}

ice::lib_lock_mutex_t::~lib_lock_mutex_t()
{
	::pthread_mutex_destroy(&this->lock_mutex);
}
