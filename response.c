#include "http.h"
#include "response.h"
#include "strutil.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>

response_info* create_response_info(void)
{
	response_info *info = (response_info*)calloc(sizeof(response_info), 1);
	return info;
}

int set_status_line(response_info *info, char* version, int code, char* phrase)
{
	info->status_line.version = (char*)calloc(sizeof(char), strlen(version) + 1);
	strcpy(info->status_line.version, version);

	info->status_line.code = code;

	info->status_line.phrase = (char*)calloc(sizeof(char), strlen(phrase) + 1);
	strcpy(info->status_line.phrase, phrase);

	return 0;
}

int add_response_header(response_info *info, char* key, char* value)
{
	header_list* p;
	header_list* item = (header_list*)calloc(sizeof(header_list), 1);
	
	item->key = (char*)calloc(sizeof(char), strlen(key) + 1);
	strcpy(item->key, key);

	item->value = (char*)calloc(sizeof(char), strlen(value) + 1);
	strcpy(item->value, value);

	if(info->headers == NULL) {
		info->headers = item;
	} else {
		for(p = info->headers; p->next != NULL; p = p->next){};
		p->next = item;
	}
	
	return 0;
}

int set_response_body(response_info *info, char* body, size_t size)
{
	char size_str[16];
	sprintf(size_str, "%zu", size);

	add_response_header(info, "content-length", size_str);

	info->body = (char*)calloc(sizeof(char), size + 1);
	memcpy(info->body, body, size);

	return 0;
}

int send_response(int fd, response_info *info)
{
	char message[1024];
	memset(message, 0, sizeof(message));

	sprintf(message, "%s %d %s\r\n%s : %s\r\n\r\n%s",
					info->status_line.version,
					info->status_line.code,
					info->status_line.phrase,
					info->headers->key,
					info->headers->value,
					info->body);

	write(fd, message, strlen(message));

	return 0;
}

