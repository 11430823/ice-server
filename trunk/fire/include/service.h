#pragma once

#include <stdint.h>
#include <netinet/in.h>
#include <map>

#include <glib/ghash.h>

#include "ice_dll.h"

class service_t
{
public:
	service_t();
	void worker_process(int bc_elem_idx, int n_inited_bc);
	fdsession_t* get_fdsess(int fd);
	struct bind_config_elem_t*	m_bind_elem;
protected:
private:
	std::map<int, fdsession_t*> fd_session_map;
};

extern service_t g_service;