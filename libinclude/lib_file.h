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
/* @brief �õ�Ŀ¼�µ��ļ�
 * @param path Ŀ¼��·��
 */
int get_current_dir_file(const char *path, std::vector<std::string>& file_names);
}//end namespace ice
