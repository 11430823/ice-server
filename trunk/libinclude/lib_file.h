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
/* @brief 得到目录下的文件
 * @param path 目录的路径
 */
int get_current_dir_file(const char *path, std::vector<std::string>& file_names);
}//end namespace ice
