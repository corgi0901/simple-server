#ifndef _REQUEST_H_
#define _REQUEST_H_

/* supported  method */
typedef enum method_type {
	GET = 0,
	METHOD_NUM
} method_type;


/* structure */
typedef struct request_line {
	method_type type;
	char* uri;
	char* version;
} request_line;

typedef struct header {
	char *key;
	char *value;
} header;

typedef struct header_list {
	header item;
	struct header_list *next;
} header_list;

typedef struct request_info {
	request_line req_line;
	header_list *headers;
	char *body;
} request_info;


void print_request_info(request_info* info);

request_info* create_request_info(void);
void release_request_info(request_info *info);

int parse_request_line(request_info *info, char* line);
int parse_request_header(request_info *info, char* header_str);
int parse_request_body(request_info *info, char* body);

int get_request(int fd, request_info *req_info);

char* get_request_header(request_info *info, char* key);

#endif
