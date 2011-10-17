#include <errno.h>
#include <dlfcn.h>
#include <error.h>

#include <stdio.h>

// #define DEBUG_LOG printf
// #define ERROR_LOG printf
// 
// #define SHOW_LOG printf


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
