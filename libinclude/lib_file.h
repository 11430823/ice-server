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

namespace ice {

//************************************
// Brief:     得到目录下的文件
// Returns:   int	0:OK 其它:失败
// Parameter: const char * path 目录的路径
// Parameter: std::vector<std::string> & file_names  返回的文件名
//************************************
int get_current_dir_file(const char *path, std::vector<std::string>& file_names);

}//end namespace ice
