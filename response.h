#ifndef _RESPONSE_H_
#define _RESPONSE_H_

#include <stdlib.h>

response_info *create_response_info(void);

int set_status_line(response_info *info, char *version, int code, char *phrase);
int add_response_header(response_info *info, char *key, char *value);
int set_response_body(response_info *info, char *body, size_t size);

int send_response(int fd, response_info *info);

#endif
