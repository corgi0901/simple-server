#ifndef _DEBUG_H_
#define _DEBUG_H_

#define DEBUG
//#undef  DEBUG

#ifdef DEBUG
	#define DPRINTF(x, __VA_ARGS__) do{ printf(x, __VA_ARGS__); }while(0);
#else
	#define DPRINTF(x, __VA_ARGS__) while(0){};
#endif

#endif
