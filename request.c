#include "http.h"
#include "request.h"
#include "strutil.h"

#include <stdio.h>
#include <malloc.h>
#include <string.h>

#define MAX_HEADER_NUM 16

static header* create_header(char* header_str)
{
	int key_len, value_len;
	char *key, *value;
	int i = 0;

	header* item = (header*)calloc(sizeof(header), 1);

	key_len = get_delim_pos(header_str, ":");
	key = (char*)calloc(sizeof(char), key_len + 1);
	memcpy(key, header_str, key_len);

	for(i = key_len; key[i] == ' '; i--) key[i] = '\0';
	convert_to_lower(key);

	item->key = (char*)calloc(sizeof(char), strlen(key) + 1);
	strcpy(item->key, key);

	value_len = (size_t)(strlen(header_str) - key_len - 1);
	value = (char*)calloc(sizeof(char), value_len + 1);
	memcpy(value, header_str + key_len + 1, value_len);

	for(i = 0; value[i] == ' '; i++){};

	strcpy(value, value + i);

	item->value = (char*)calloc(sizeof(char), value_len + 1);
	strcpy(item->value, value);

	free(key);
	free(value);

	return item;
}

void print_request_info(request_info* info)
{
	header_list *item;

	printf("----- Request Line -----\n");
	printf("method  : %d\n", info->req_line.type);
	printf("uri     : %s\n", info->req_line.uri);
	printf("version : %s\n", info->req_line.version);

	printf("----- Request Header -----\n");
	for(item = info->headers; item != NULL; item = item->next) {
		printf("%s : %s\n", item->item.key, item->item.value);
	}

	printf("----- Request Body -----\n");
	if(info->body != NULL) {
		printf("%s\n\n", info->body);
	} else {
		printf("(NULL)\n\n");
	}
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
	char *item_str;
	header *item;
	header_list *last;

	for(count = 0; token != NULL && count < MAX_HEADER_NUM; count++) {
		tokens[count] = token;
		token = strtok(NULL, CRLF);
	}

	for(i = 0; i < count; i++) {
		item_str = (char*)calloc(sizeof(char), strlen(tokens[i]) + 1);
		strcpy(item_str, tokens[i]);

		item = create_header(item_str);

		header_list *p = (header_list*)calloc(sizeof(header_list), 1);
		p->item = *item;
		p->next = NULL;

		if(info->headers == NULL){
			info->headers = p;
			last = p;
		} else {
			last->next = p;
			last = p;
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

char* get_request_header(request_info *info, char* key)
{
	header_list *header = info->headers;

	for(header = info->headers; header != NULL; header = header->next) {
		if(0 == strcmp(header->item.key, key)) {
			return header->item.value;
		}
	}

	return NULL;
}

