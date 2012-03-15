#include <errno.h>
#include <dlfcn.h>
#include <error.h>
#include <stdio.h>
#include <cstdlib>
#include <algorithm>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iostream>
#include <iterator>
#include "../libinclude/astar.h"
using namespace std;

// #define DEBUG_LOG printf
// #define ERROR_LOG printf
// 
// #define SHOW_LOG printf

/*
#define DLFUNC(h, v, name, type) \
	do { \
	void* c = dlsym (h, name); \
	ERROR_LOG("xxx %p", c);\
	v = (type)c;\
	if ((error = dlerror ()) != NULL) { \
	ERROR_LOG("dlsym error, %s %p", error, v);\
	dlclose(h); \
	h = NULL; \
	goto out; \
	} \
	} while (0)

void	(*cc)();
void*   handle;
int main(int argc, char* argv[])
{
	char* error = NULL; 
	handle = dlopen("./liblogic.so", RTLD_NOW);
	if ((error = dlerror()) != NULL) {
		ERROR_LOG("dlopen error111, %s \r\n", error);
		goto out;
	}


	DLFUNC(handle, cc, "on_events", void(*)());
out:
	return 0;
}
*/

int main()
{
	AStar<> astar("./map.dat");
	if (!astar) {
		cout << "oops" << endl;
		return 0;
	}
	cout << "fine" << endl;

	//	const AStar<>::Points* pts = astar.findpath(AStar<>::Point(350, 210), AStar<>::Point(160, 0));
	for (int i = 0; i<1; i++){
		const AStar<>::Points* pts = astar.findpath(AStar<>::Point(900, 900), AStar<>::Point(0, 900));
 		for (AStar<>::Points::const_iterator it = pts->begin(); it != pts->end(); ++it) {
 			cout << '(' << it->x << ", " << it->y << ')' << ", ";
 		}
 		cout << endl;
	}


	//copy(pts->begin(), pts->end(), ostream_iterator<string>(outfile, "\n") );
	return 0;
}
