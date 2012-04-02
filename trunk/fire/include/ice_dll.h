/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	接口
	brief:		ok
*********************************************************************/

#pragma once

#include <sys/types.h>
#include <netinet/in.h>

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

class ice_dll_t
{
public:
	ice_dll_t();
	virtual ~ice_dll_t();
	//************************************
	// Brief:	注册插件
	// Returns:	int 0:success -1:error
	//************************************
	int  register_plugin();

//The following interfaces are called only by the child process
	//************************************
	// Brief:	Called each time before processing packages from clients.
	//			Calling interval of this interface is no much longer than 100ms at maximum.
	//************************************
	void (*on_events)();
	//************************************
	// Brief:  	Called to process packages from clients. Called once for each package.
	// 			Return non-zero if you want to close the client connection from which the `pkg` is sent,
	// 			otherwise returns 0. If non-zero is returned, `on_cli_conn_closed` will be called too. 
	//************************************
	int (*on_cli_pkg)(void* pkg, int pkglen, fdsession_t* fdsess);
	//************************************
	// Brief:	Called to process packages from servers that the child connects to. Called once for each package.
	//************************************
	void (*on_srv_pkg)(int fd, void* pkg, int pkglen);
	//************************************
	// Brief:	Called each time when a client close a connection, or when `on_cli_pkg` returns non-zero.
	//************************************
	void (*on_cli_conn_closed)(int fd);
	//************************************
	// Brief:	Called each time on close of the FDs opened by the child.
	//************************************
	void (*on_fd_closed)(int fd);

//The following interfaces are called both by the parent and child process
	//************************************
	// Brief:	//Called only once at server startup by both the parent and child process.
				//`isparent == 1` indicates this interface is called by the parent;
				//`isparent == 0` indicates this interface is called by the child.
				//You should initialize your service program (allocate memory, create objects, etc) here.
				//You must return 0 on success, -1 otherwise.
   	//************************************
	int (*on_init)(int isparent);
	//************************************
	// Brief:	//Called only once at server stop by both the parent and child process.
				//`isparent == 1` indicates this interface is called by the parent;
				//`isparent == 0` indicates this interface is called by the child.
				//You should finalize your service program (release memory, destroy objects, etc) here.
				//You must return 0 if you have finished finalizing the service, -1 otherwise. 
	//************************************
	int (*on_fini)(int isparent);
	/*!
	  * This interface will be called both by the parent and child process.
	  * `isparent == 1` indicates this interface is called by the parent;
	  * `isparent == 0` indicates this interface is called by the child.
	  * You must return 0 if you cannot yet determine the length of the incoming package,
	  * return -1 if you find that the incoming package is invalid and ice-server will close the connection,
	  * otherwise, return the length of the incoming package. Note, the package should be no larger than 8192 bytes.
	  */
	int	(*on_get_pkg_len)(int fd, const void* avail_data, int avail_len, int isparent);
protected:
private:
	void* m_handle;
};

extern ice_dll_t g_dll;