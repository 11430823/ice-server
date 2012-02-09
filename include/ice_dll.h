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

struct fdsession_t;

class ice_dll_t
{
public:
	ice_dll_t();
	~ice_dll_t();
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
	// 			otherwise returns 0. If non-zero is returned, `on_client_conn_closed` will be called too. 
	//************************************
	int (*on_cli_pkg)(void* pkg, int pkglen, fdsession_t* fdsess);
	//************************************
	// Brief:	Called to process packages from servers that the child connects to. Called once for each package.
	//************************************
	void (*on_srv_pkg)(int fd, void* pkg, int pkglen);
	//************************************
	// Brief:	Called to process multicast packages from the specified `mcast_ip` and `mcast_port`. Called once for each package.
	//************************************
	void (*on_mcast_pkg)(const void* data, int len);
	//************************************
	// Brief:	Called each time when a client close a connection, or when `proc_pkg_from_client` returns -1.
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

	//mark未建UDP的socket创建.
	int	(*on_udp_pkg)(int fd, const void* avail_data, int avail_len ,struct sockaddr_in * from, socklen_t fromlen );
	//mark构建机制
    //************************************
    // Brief:	//This interface is used to sync a server's name, ip, and port to business login.\n
				//arg 'svr_id': server id \n
				//arg 'svr_name': '\0' terminated, max length 16 ('\0' included) \n
				//arg 'svr_ip': '\0' terminated, max length 16 ('\0' included) \n
				//arg 'port': port \n
				//arg 'flag': 0 - delete, 1 - add   
    //************************************
    void (*on_sync_srv_info)(uint32_t svr_id, const char* svr_name, const char* svr_ip, in_port_t port, int flag);
protected:
private:
	void* m_handle;
};

extern ice_dll_t g_dll;