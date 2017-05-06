#include "strutil.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>

void convert_to_lower(char* str)
{
	int i = 0;
	for(i = 0; str[i] != '\0'; i++) {
		str[i] = tolower(str[i]);
	}
}

int get_delim_pos(char* str, char *delim)
{
	char *pos = strstr(str, delim);

	if(pos == NULL) {
		return -1;
	} else {
		return (int)(pos - str);
	}
}
