/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		ok
*********************************************************************/

#pragma once

#include "lib_include.h"
#include "lib_util.h"

namespace ice{
	class lib_file_t{
	public:	
		//************************************
		// Brief:     Set the given fd to be blocking or noblocking
		// Returns:   int	0 on success, -1 on error.
		// Parameter: int fd	file descriptor to be set.
		// Parameter: bool is_block	true:blocking, false:nonblocking
		//************************************
		static inline int set_io_block(int fd, bool is_block){
			int val;
			if (is_block) {
				val = (~O_NONBLOCK & ::fcntl(fd, F_GETFL));
			} else {
				val = (O_NONBLOCK | ::fcntl(fd, F_GETFL));
			}
			return ::fcntl(fd, F_SETFL, val);
		}
		//************************************
		// Brief:	  close the given fd(if fd is VALID),and set to be -1 
		// Returns:   int (0 on success, -1 on error)
		// Parameter: int & s (fd)
		// 其中判定 -1 != fd  时才可关闭
		//************************************
		static inline int close_fd(int& fd){
			int nRes = 0;
			if (-1 != fd){
				nRes = HANDLE_EINTR(::close(fd));
				if(-1 != nRes){
					fd = -1;
				}
			}
			return nRes;
		}

		//************************************
		// Brief:     得到目录下的文件
		// Returns:   int	0:OK 其它:失败
		// Parameter: const char * path 目录的路径
		// Parameter: std::vector<std::string> & file_names  返回的文件名
		//************************************
		static int get_dir_file(const char *path, std::vector<std::string>& file_names);
	protected:
	private:
		lib_file_t(const lib_file_t& cr);
		lib_file_t& operator=(const lib_file_t& cr);
	};

}//end namespace ice