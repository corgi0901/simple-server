#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>

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
	int len;
	int ret;

	const char* response = "HTTP1.1 200 OK";
	
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
		/* listen socket */
		listen(rsock, WAIT_QUEUE_LEN);

		/* accept TCP connection from client */
		len = sizeof(client);
		wsock = accept(rsock, (struct sockaddr *)&client, &len);

		/* send message */
		write(wsock, response, sizeof(response));

		/* close TCP session */
		close(wsock);
	}

	close(rsock);

	return 0;
}
