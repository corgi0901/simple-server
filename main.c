#include "request.h"
#include "response.h"
#include "http.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <unistd.h>

#define PORT 8080
#define WAIT_QUEUE_LEN 5

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

int main(void) {

	int rsock, wsock;
	struct sockaddr_in addr, client;
	socklen_t len;
	int ret;

	char *response = "Hello World";

	request_info *req_info;
	response_info *res_info;

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
		res_info = create_response_info();

		/* listen socket */
		listen(rsock, WAIT_QUEUE_LEN);

		/* accept TCP connection from client */
		len = sizeof(client);
		wsock = accept(rsock, (struct sockaddr *)&client, &len);

		/* parse request message */
		get_request(wsock, req_info);

		/* set response message */
		set_status_line(res_info, "HTTP/1.1", 200, "OK");
		set_response_body(res_info, response, strlen(response));

		/* send message */
		send_response(wsock, res_info);

		/* close TCP session */
		close(wsock);
		release_request_info(req_info);
	}

	close(rsock);

	return 0;
}
