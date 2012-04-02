#include "lib_file.h"
#include "lib_net.h"

ice::lib_net_t::lib_net_t()
{
	this->fd = -1;
}

ice::lib_net_t::~lib_net_t()
{
	lib_file_t::close_fd(this->fd);
}
