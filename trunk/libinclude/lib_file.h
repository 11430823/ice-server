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
// Brief:     �õ�Ŀ¼�µ��ļ�
// Returns:   int	0:OK ����:ʧ��
// Parameter: const char * path Ŀ¼��·��
// Parameter: std::vector<std::string> & file_names  ���ص��ļ���
//************************************
int get_current_dir_file(const char *path, std::vector<std::string>& file_names);

}//end namespace ice
