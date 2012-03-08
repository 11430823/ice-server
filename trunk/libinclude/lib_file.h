/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		ok
*********************************************************************/

#pragma once

#include <vector>
#include <string>

namespace ice{
	class lib_file_t
	{
	public:
		lib_file_t(){}
		//virtual ~lib_file_t();	
		//************************************
		// Brief:     Set the given fd to be blocking or noblocking
		// Returns:   int	0 on success, -1 on error.
		// Parameter: int fd	file descriptor to be set.
		// Parameter: bool is_block	true:blocking, false:nonblocking
		//************************************
		static int set_io_block(int fd, bool is_block);
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