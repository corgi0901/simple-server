#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define WAIT_QUEUE_LEN 5

int main(void) {

	int rsock, wsock;
	struct sockaddr_in addr, client;
	int len;
	int ret;

	const char* response = "HTTP1.1 200 OK";
	
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

	/* listen socket */
	listen(rsock, WAIT_QUEUE_LEN);

	/* accept TCP connection from client */
	len = sizeof(client);
	wsock = accept(rsock, (struct sockaddr *)&client, &len);

	/* send message */
	write(wsock, response, sizeof(response));

	/* close TCP session */
	close(wsock);
	close(rsock);

	return 0;
}
