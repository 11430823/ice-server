#pragma once

#include <stdint.h>
#include <netinet/in.h>
#include <map>

#include <glib/ghash.h>

#pragma pack(1)

struct fdsession_t {
	int			fd;
	in_port_t	remote_port;
	uint32_t	remote_ip;
	uint8_t		type;
	uint8_t		flag;
	time_t		last_tm;
};

#pragma pack()

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