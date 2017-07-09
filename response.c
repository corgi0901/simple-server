#include "http.h"
#include "response.h"
#include "strutil.h"
#include "debug.h"

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

	item->next = NULL;

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

#define BUFF_UNIT 1024
int send_response(int fd, response_info *info)
{
	size_t buf_size = BUFF_UNIT;
	size_t buf_len  = 0;
	char *send_buff = (char*)calloc(sizeof(char), buf_size);

	char status_line[512];
	size_t status_size;

	char header[512];
	size_t header_size;

	size_t crlf_size;
	size_t body_size;

	memset(status_line, 0, sizeof(status_line));
	sprintf(status_line, "%s %d %s"CRLF, info->status_line.version, info->status_line.code, info->status_line.phrase);
	status_size = strlen(status_line);

	/* create buffer if needed */
	while(buf_size - buf_len <= status_size){
		buf_size += BUFF_UNIT;
		send_buff = (char*)realloc(send_buff, sizeof(char) * buf_size);
	}

	/* set status line of response */
	DPRINTF("--- status line ---\n%s", status_line);
	memcpy(send_buff + buf_len, status_line, status_size);
	buf_len += status_size;

	/* add header */
	DPRINTF("%s", "--- response header ---\n");
	header_list *item = info->headers;
	while(item != NULL) {
		memset(header, 0, sizeof(header));
		sprintf(header, "%s:%s"CRLF, item->key, item->value);
		header_size = strlen(header);

		while(buf_size - buf_len <= header_size){
			buf_size += BUFF_UNIT;
			send_buff = (char*)realloc(send_buff, sizeof(char) * buf_size);
		}

		DPRINTF("%s", header);
		memcpy(send_buff + buf_len, header, header_size);
		buf_len += header_size;

		item = item->next;
	}

	/* add delimiter between header and body */
	crlf_size = strlen(CRLF);
	while(buf_size - buf_len <= crlf_size){
		buf_size += BUFF_UNIT;
		send_buff = (char*)realloc(send_buff, sizeof(char) * buf_size);
	}

	memcpy(send_buff + buf_len, CRLF, crlf_size);
	buf_len += crlf_size;

	/* add body */
	body_size = strlen(info->body);
	while(buf_size - buf_len <= body_size){
		buf_size += BUFF_UNIT;
		send_buff = (char*)realloc(send_buff, sizeof(char) * buf_size);
	}

	DPRINTF("--- response body ---\n%s\n", info->body);
	memcpy(send_buff + buf_len, info->body, body_size);
	buf_len += body_size;

	write(fd, send_buff, buf_len);
	free(send_buff);

	return 0;
}
#undef BUFF_UNIT

