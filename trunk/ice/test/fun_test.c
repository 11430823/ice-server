#include <iostream>
int (*fn)();
int add()
{
	std::cout<<fn<<std::endl; 
	fn = 0;
	std::cout<<fn<<std::endl; 
	return 8;
}

int main()
{
	fn = add;
	int i = fn();
	std::cout<<i<<std::endl; 
	i = fn();
	std::cout<<"ff:"<<i<<std::endl; 
}
