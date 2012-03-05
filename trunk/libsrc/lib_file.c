#include <dirent.h>
#include <fcntl.h>

#include "lib_file.h"
#include "lib_log.h"

int ice::set_io_block( int fd, bool is_block )
{
	int val;
	if (is_block) {
		val = (~O_NONBLOCK & fcntl(fd, F_GETFL));
	} else {
		val = (O_NONBLOCK | fcntl(fd, F_GETFL));
	}
	return fcntl(fd, F_SETFL, val);
}

int ice::get_dir_file( const char *path, std::vector<std::string>& file_names )
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
		KERROR_LOG(0, "open fail [path:%s]", path);
		return -1;
	}	
	return 0;
}
