#include "strutil.h"
#include <string.h>

void remove_char(char* dst, char* src, char c)
{
	int length = strlen(src);
	int i = 0;
	int pos = 0;

	for(i = 0; i < length; i++) {
		if(src[i] != c) {
			dst[pos] = src[i];
			pos++;
		}
	}

	dst[pos] = '\0';
}
