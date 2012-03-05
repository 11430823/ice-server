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
	//************************************
	// Brief:     Set the given fd to be blocking or noblocking
	// Returns:   int	0 on success, -1 on error.
	// Parameter: int fd	file descriptor to be set.
	// Parameter: bool is_block	true:blocking, false:nonblocking
	//************************************
	int set_io_block(int fd, bool is_block);
	//************************************
	// Brief:     �õ�Ŀ¼�µ��ļ�
	// Returns:   int	0:OK ����:ʧ��
	// Parameter: const char * path Ŀ¼��·��
	// Parameter: std::vector<std::string> & file_names  ���ص��ļ���
	//************************************
	int get_dir_file(const char *path, std::vector<std::string>& file_names);
}


