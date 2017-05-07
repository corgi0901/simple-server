#include "request.h"
#include "http.h"
#include "strutil.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <errno.h>

#define PORT 8080
#define WAIT_QUEUE_LEN 5

#define UNIT_SIZE 512

/* proto type function */
static void set_signal_handler(int signame);
static void signal_handler(int signame);

static void set_signal_handler(int signame)
{
	if (SIG_ERR == signal(signame, signal_handler)) {
		fprintf(stderr, "Error. Cannot set signal handler.\n");
		exit(1);
	}
}

static void signal_handler(int signame)
{
	printf("simple-server stopped.\n");
	exit(0);
}

static int get_request(int fd, request_info *req_info)
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

	parse_request_line(req_info, data_pool);

	remain_data = delim_pos + strlen(CRLF);
	strcpy(data_pool, remain_data);
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

	parse_request_body(req_info, data_pool);

end:
	print_request_info(req_info);
	memset(data_pool, 0, pool_size + 1);
	free(data_pool);
}

int main(void) {

	int rsock, wsock;
	struct sockaddr_in addr, client;
	int len;
	int ret;

	const char *response = "HTTP1.1 200 OK";

	request_info *req_info;

	set_signal_handler(SIGINT);
	printf("simple-server started.\n");

	/* make socket */
	rsock = socket(AF_INET, SOCK_STREAM, 0);

	if (rsock < 0) {
		fprintf(stderr, "Error. Cannot make socket\n");
		return -1;
	}

	/* socket setting */
	addr.sin_family      = AF_INET;
	addr.sin_port        = htons(PORT);
	addr.sin_addr.s_addr = INADDR_ANY;

	/* binding socket */	
	ret = bind(rsock, (struct sockaddr *)&addr, sizeof(addr));

	if (ret < 0) {
		fprintf(stderr, "Error. Cannot bind socket\n");
		return -1;
	}

	while(1) {

		req_info = create_request_info();

		/* listen socket */
		listen(rsock, WAIT_QUEUE_LEN);

		/* accept TCP connection from client */
		len = sizeof(client);
		wsock = accept(rsock, (struct sockaddr *)&client, &len);

		/* parse request message */
		get_request(wsock, req_info);

		/* send message */
		write(wsock, response, sizeof(response));

		/* close TCP session */
		close(wsock);
		release_request_info(req_info);
	}

	close(rsock);

	return 0;
}
