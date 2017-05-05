#include "../request.h"

int main(void) {
	char req_line[]   = "GET /index.html HTTP1.1";
	char header_str[] = "aaa : bbb\r\nccc:ddd\r\neee : fff";
	char body_str[]   = "Hello World";

	request_info *info = create_request_info();
	
	parse_request_line(info, req_line);
	parse_request_header(info, header_str);
	parse_request_body(info, body_str);

	release_request_info(info);

	return 0;
}
