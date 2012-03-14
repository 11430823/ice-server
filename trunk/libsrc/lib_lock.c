#include "lib_lock.h"

ice::lib_lock_mutex::lib_lock_mutex()
{
	pthread_mutex_init(&this->lock_mutex,NULL);
}

ice::lib_lock_mutex::~lib_lock_mutex()
{
	pthread_mutex_destroy(&this->lock_mutex);
}

void ice::lib_lock_mutex::lock()
{
	pthread_mutex_lock(&this->lock_mutex);
}

void ice::lib_lock_mutex::ulock()
{
	pthread_mutex_unlock(&this->lock_mutex);
}
