#include "http.h"
#include "request.h"
#include "strutil.h"

#include <stdio.h>
#include <malloc.h>
#include <string.h>

#define MAX_HEADER_NUM 16

static header* create_header(char* key, char* value) 
{
	header* item = (header*)calloc(sizeof(header), 1);

	item->key = (char*)calloc(sizeof(char), strlen(key) + 1);
	strcpy(item->key, key);

	item->value = (char*)calloc(sizeof(char), strlen(value) + 1);
	strcpy(item->value, value);

	return item;
}



request_info* create_request_info(void) 
{
	request_info *info = (request_info*)calloc(sizeof(request_info), 1);
	return info;
}

void release_request_info(request_info *info)
{
	header_list *p, *temp;

	/* free request line */
	free(info->req_line.uri);
	free(info->req_line.version);

	/* free request header */
	p = info->headers;
	while(p != NULL) {
		free(p->item.key);
		free(p->item.value);
		temp = p;
		p = p->next;
		free(temp);
	}

	/* free request body */
	free(info->body);

	/* free request info */
	free(info);
}

int parse_request_line(request_info *info, char* line) 
{
	char *method, *uri, *version;

	method = strtok(line, " ");
	if(method == NULL) return -1;

	if(strcmp("GET", method) == 0) {
		info->req_line.type = GET;
	}

	uri = strtok(NULL, " ");
	if(uri == NULL) return -1;

	info->req_line.uri = (char*)calloc(sizeof(char), strlen(uri) + 1);
	strcpy(info->req_line.uri, uri);

	version = strtok(NULL, " ");
	if(version == NULL) return -1;

	info->req_line.version = (char*)calloc(sizeof(char), strlen(version) + 1);
	strcpy(info->req_line.version, version);

	return 0;
}

int parse_request_header(request_info *info, char* header_str)
{
	char* tokens[MAX_HEADER_NUM];
	int count = 0;

	char* token = strtok(header_str, CRLF);

	int i;
	char *item_str, *key, *value;
	header *item;

	for(count = 0; token != NULL && count < MAX_HEADER_NUM; count++) {
		tokens[count] = token;
		token = strtok(NULL, CRLF);
	}

	for(i = 0; i < count; i++) {
		item_str = (char*)calloc(sizeof(char), strlen(tokens[i]) + 1);
		remove_char(item_str, tokens[i], ' ');

		key = strtok(item_str, ":");
		value = strtok(NULL, ":");

		item = create_header(key, value);

		header_list *p = (header_list*)calloc(sizeof(header_list), 1);
		p->item = *item;

		if(info->headers == NULL){
			info->headers = p;
		} else {
			p->next = info->headers;
			info->headers = p;
		}

		free(item_str);
	}
	
	return 0;
}

int parse_request_body(request_info *info, char* body)
{
	info->body = (char*)calloc(sizeof(char), strlen(body) + 1);
	strcpy(info->body, body);
	return 0;
}

