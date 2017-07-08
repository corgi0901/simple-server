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

void lstrip(char* str) {
	int i = 0, j = 0;

	for(i = 0; str[i] != '\0'; i++) {
		if(str[i] != ' '){
			break;
		}
	}

	if(str[i] == '\0') return;

	j = 0;
	while(str[i + j] != '\0') {
		str[j] = str[i + j];
		j++;
	}
	str[j] = '\0';

	return;
}

void rstrip(char* str) {
	int i = 0;
	int len = strlen(str);

	for(i = len - 1; str[i] == ' '; i--) {
		str[i] = '\0';
	}

	return;
}

void strip(char *str) {
	lstrip(str);
	rstrip(str);
	return;
}
