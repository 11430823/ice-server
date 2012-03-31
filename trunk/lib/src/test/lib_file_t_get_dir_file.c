#include <dirent.h>
#include <iostream>
#include <vector>
#include <string>

int get_dir_file( const char *path, std::vector<std::string>& file_names )
{
	DIR* dirp;
	struct dirent* direntp;
	dirp = ::opendir(path);
	if (dirp != NULL) {
		while(1) {
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

int main(){
	std::vector<std::string> file_name;
	get_dir_file("/home/meng/", file_name);
	for(int i = 0; i < file_name.size(); i++){
		std::cout<<file_name[i].c_str()<<std::endl;
	}
	return 0;
}