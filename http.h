#ifndef _HTTP_H_
#define _HTTP_H_

#define SP " "
#define CRLF "\r\n"
#define HEADER_END "\r\n\r\n"

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

typedef struct status_line {
	char* version;
	int code;
	char* phrase;
} status_line;

typedef struct header_list {
	char *key;
	char *value;
	struct header_list *next;
} header_list;

typedef struct request_info {
	request_line req_line;
	header_list *headers;
	char *body;
} request_info;

typedef struct response_info {
	status_line status_line;
	header_list *headers;
	char* body;
} response_info;

#endif
