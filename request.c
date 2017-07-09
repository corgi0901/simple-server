#include "http.h"
#include "request.h"
#include "strutil.h"
#include "debug.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define MAX_HEADER_NUM 16
#define UNIT_SIZE 512

static header_list* create_header(char* header_str)
{
	int key_len, value_len;
	char *key, *value;
	int i = 0;

	header_list* item = (header_list*)calloc(sizeof(header_list), 1);

	key_len = get_delim_pos(header_str, ":");
	key = (char*)calloc(sizeof(char), key_len + 1);
	memcpy(key, header_str, key_len);

	strip(key);
	convert_to_lower(key);

	item->key = (char*)calloc(sizeof(char), strlen(key) + 1);
	strcpy(item->key, key);

	value_len = (size_t)(strlen(header_str) - key_len - 1);
	value = (char*)calloc(sizeof(char), value_len + 1);
	memcpy(value, header_str + key_len + 1, value_len);

	strip(value);

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
		printf("%s : %s\n", item->key, item->value);
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
		free(p->key);
		free(p->value);
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
	header_list *item, *last;

	for(count = 0; token != NULL && count < MAX_HEADER_NUM; count++) {
		tokens[count] = token;
		token = strtok(NULL, CRLF);
	}

	for(i = 0; i < count; i++) {
		item_str = (char*)calloc(sizeof(char), strlen(tokens[i]) + 1);
		strcpy(item_str, tokens[i]);

		item = create_header(item_str);

		if(info->headers == NULL){
			info->headers = item;
			last = item;
		} else {
			last->next = item;
			last = item;
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
		if(0 == strcmp(header->key, key)) {
			return header->value;
		}
	}

	return NULL;
}

int get_request(int fd, request_info *req_info)
{
	int pool_size = UNIT_SIZE;
	char* data_pool =(char*)calloc(sizeof(char), pool_size + 1);

	int len = 0;
	int recv_size = 0;

	char* delim_pos = NULL;
	char* remain_data;

	char* content_length;
	int body_len;

	/* read request line */
	while(1) {
		if(pool_size < recv_size + UNIT_SIZE) {
			pool_size += UNIT_SIZE;
			data_pool = (char*)realloc(data_pool, sizeof(char) * (pool_size + 1));
		}

		len = read(fd, data_pool + recv_size, UNIT_SIZE);
		if(len < 0) {
			fprintf(stderr, "Error. read(%d) %s\n", errno, strerror(errno));
			exit(1);
		}
		recv_size += len;

		delim_pos = strstr(data_pool, CRLF);

		if(delim_pos) {
			*delim_pos = '\0';
			break;
		}
	}

	DPRINTF("--- request-line ---\n%s\n", data_pool);
	parse_request_line(req_info, data_pool);

	remain_data = delim_pos + strlen(CRLF);
	char* tmp = (char*)calloc(sizeof(char), strlen(remain_data) + 1);
	strcpy(tmp, remain_data);
	strcpy(data_pool, tmp);
	free(tmp);

	recv_size = strlen(remain_data);

	/* read request header */
	while(1) {
		delim_pos = strstr(data_pool, HEADER_END);

		if(delim_pos) {
			*delim_pos = '\0';
			break;
		}

		if(pool_size < recv_size + UNIT_SIZE) {
			pool_size += UNIT_SIZE;
			data_pool = (char*)realloc(data_pool, sizeof(char) * (pool_size + 1));
		}

		len = read(fd, data_pool + recv_size, UNIT_SIZE);
		if(len < 0) {
			fprintf(stderr, "Error. read(%d) %s\n", errno, strerror(errno));
			exit(1);
		}
		recv_size += len;
	}

	DPRINTF("--- request header ---\n%s\n", data_pool);
	parse_request_header(req_info, data_pool);

	remain_data = delim_pos + strlen(HEADER_END);
	strcpy(data_pool, remain_data);
	recv_size = strlen(remain_data);

	content_length = get_request_header(req_info, "content-length");

	if(content_length == NULL) {
		goto end;
	} else {
		body_len = atoi(content_length);
	}

	while(recv_size < body_len) {
		if(pool_size < recv_size + UNIT_SIZE) {
			pool_size += UNIT_SIZE;
			data_pool = (char*)realloc(data_pool, sizeof(char) * (pool_size + 1));
		}

		len = read(fd, data_pool + recv_size, UNIT_SIZE);
		if(len < 0) {
			fprintf(stderr, "Error. read(%d) %s\n", errno, strerror(errno));
			exit(1);
		}
		recv_size += len;
	}

	DPRINTF("--- request body ---\n%s\n", data_pool);
	parse_request_body(req_info, data_pool);

end:
	memset(data_pool, 0, pool_size + 1);
	free(data_pool);

	return 0;
}

