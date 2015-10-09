#ifndef __UTILS_H__
#define __UTILS_H__

#define DEBUG 0

#define error_print(fmt, ...) \
        do { fprintf(stderr, "ERROR: " fmt, \
	                                __VA_ARGS__); } while (0)

#define debug_print(fmt, ...) \
        do { if (DEBUG) fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
	                                __LINE__, __func__, __VA_ARGS__); } while (0)

#endif
