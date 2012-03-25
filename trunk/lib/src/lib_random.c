#include <stdlib.h>

#include "lib_random.h"

int ice::lib_random_t::random( int min, int max )
{
	return (::rand()%(max-min+1))+min;
}

int ice::lib_random_t::hash( const char* p )
{
	int h = 0;
	while (*p) {
		h = h * 11 + (*p << 4) + (*p >> 4);
		p++;
	}
	return h;
}
