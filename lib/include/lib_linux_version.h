#include <linux/version.h>

#if LINUX_VERSION_CODE>= KERNEL_VERSION(2,6,36)
/*......codes under version newer than 2.6.36......*/
#else
/*......codes under version older than 2.6.36......*/
#endif
