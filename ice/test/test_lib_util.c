#include "../libinclude/lib_util.h"
/*
inline void convert_from_string(T &value, const std::string &s) {
	std::stringstream ss(s);
	ss >> value;
}
*/
int main(){

/*
	std::string x;
	std::stringstream ss;
	ss << "x y z";
	ss >> x;
	convert_from_string(x, "x1y z");	 
	std::cout << x;
*/
std::stringstream ss;
int x;
ss << "";
ss >> x;
std::cout << x;
}
