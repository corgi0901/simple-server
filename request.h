#ifndef _REQUEST_H_
#define _REQUEST_H_

#include "http.h"

void print_request_info(request_info* info);

request_info* create_request_info(void);
void release_request_info(request_info *info);

int parse_request_line(request_info *info, char* line);
int parse_request_header(request_info *info, char* header_str);
int parse_request_body(request_info *info, char* body);

int get_request(int fd, request_info *req_info);

char* get_request_header(request_info *info, char* key);

#endif
