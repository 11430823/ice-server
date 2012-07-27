#include "lib_file.h"

int ice::lib_file_t::get_dir_file( const char *path, std::vector<std::string>& file_names )
{
	DIR* dirp;
	dirp = ::opendir(path);
	if (NULL != dirp) {
		while(1) {
			struct dirent* direntp;
			direntp = ::readdir(dirp);
			if ( NULL == direntp ) {
				break;
			}
			if (DT_REG == direntp->d_type) {
				std::string name = direntp->d_name;
				file_names.push_back(name);
			}
		}
		::closedir(dirp);
	} else {
		return -1;
	}	
	return 0;
}
