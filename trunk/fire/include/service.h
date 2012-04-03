#pragma once

#include <stdint.h>
#include <netinet/in.h>
#include <map>

#include <glib/ghash.h>

#include "dll.h"

class service_t
{
public:
	service_t();
	void run(int bc_elem_idx, int n_inited_bc);
	struct bind_config_elem_t*	m_bind_elem;
protected:
private:
};

extern service_t g_service;