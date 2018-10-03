#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stdio.h>

#define DEBUG
//#undef  DEBUG

#ifdef DEBUG
#define DPRINTF(x, ...)         \
	do                          \
	{                           \
		printf(x, __VA_ARGS__); \
	} while (0);
#else
#define DPRINTF(x, ..) \
	while (0)          \
	{                  \
	};
#endif

#endif
