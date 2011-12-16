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
#include <dirent.h>

/* @brief 得到目录下的文件
 * @param path 目录的路径
 */
int get_current_dir_file(const char *path, std::vector<std::string>& file_names)
{
	DIR* dirp;
	struct dirent* direntp;
	dirp = opendir(path);
	if (dirp != NULL) {
		while(1) {
			direntp = readdir(dirp);
			if ( NULL == direntp ) {
				break;
			}
			if (DT_REG == direntp->d_type) {
				std::string name = direntp->d_name;
				file_names.push_back(name);
			}
		}
		closedir(dirp);
	} else {
		KERROR_LOG(0, "open fail");
		return -1;
	}	
	return 0;
}