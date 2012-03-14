#include <stdlib.h>
#include <string.h>

#include "lib_memory.h"

void ice::lib_memory_active_buf::init_data()
{
	this->data = NULL;
	this->total_len = 0;
	this->write_pos = 0;
}

void ice::lib_memory_active_buf::clean()
{
	free(this->data);
	this->init_data();
}

uint32_t ice::lib_memory_active_buf::pop_front( uint32_t len )
{
	if(this->write_pos <= len){
		//È«²¿µ¯³ö
		this->clean();
	}else{
		memmove(this->data, this->data + len, this->write_pos - len);
		this->write_pos -= len;
	}
	return this->write_pos;
}

void ice::lib_memory_active_buf::push_back( const char* const pdata, uint32_t len )
{
	uint32_t remain_len = this->total_len - this->write_pos;
	if(remain_len >= len){
		memcpy(this->data + this->write_pos, pdata, len);
		this->write_pos += len;
	}else{
		if(this->total_len == 0){
			this->data = (char*)malloc(len);
			memcpy(this->data, pdata, len);
			this->total_len = len;
			this->write_pos = len;
		}else{
			this->data = (char*)realloc(this->data, len + this->write_pos);
			memcpy(this->data + this->write_pos, pdata, len);
			this->total_len = len + this->write_pos;
			this->write_pos = this->total_len;
		}
	}
}

ice::lib_memory_active_buf::lib_memory_active_buf()
{
	this->init_data();
}

ice::lib_memory_active_buf::~lib_memory_active_buf()
{
	this->clean();
}

