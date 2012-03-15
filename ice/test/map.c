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

int main()
{
	AStar<> astar("./map.dat");
	if (!astar) {
		cout << "oops" << endl;
		return 0;
	}
	cout << "fine" << endl;

	//	const AStar<>::Points* pts = astar.findpath(AStar<>::Point(350, 210), AStar<>::Point(160, 0));
	for (int i = 0; i<10000; i++){
		const AStar<>::Points* pts = astar.findpath(AStar<>::Point(900, 900), AStar<>::Point(0, 900));
		/*
 		for (AStar<>::Points::const_iterator it = pts->begin(); it != pts->end(); ++it) {
 			cout << '(' << it->x << ", " << it->y << ')' << ", ";
 		}
 		cout << endl;
		*/
	}


	//copy(pts->begin(), pts->end(), ostream_iterator<string>(outfile, "\n") );
	return 0;
}
