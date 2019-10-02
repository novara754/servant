#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define ADDRESS "127.0.0.1"
#define LISTEN_BACKLOG 50

char response[] =
	"HTTP/1.1 200 OK\n"
	"Content-Type: text/html\n"
	"\n"
	"<html>"
	"<head>"
		"<title>Servant Home</title>"
	"</head>"
	"<body>"
		"<h1>Servant Home</h1>"
	"</body>"
	"</html>";

int create_server(void);
void accept_client(int server_socket);

int main() {
	int server = create_server();
	while (1) {
		accept_client(server);
	}
	return 0;
}

int create_server(void) {
	int server = socket(AF_INET, SOCK_STREAM, 0);
	if (server < 0) {
		perror("failed to create socket");
		exit(1);
	}

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);

	if (bind(server, (struct sockaddr*) &addr, sizeof(addr)) != 0) {
		perror("failed to bind socket");
		exit(1);
	}

	if (listen(server, LISTEN_BACKLOG) == -1) {
		perror("failed to listen");
		exit(1);
	}

	return server;
}

void accept_client(int server_socket) {
	struct sockaddr_in client_addr;
	socklen_t client_addr_len;
	int client = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
	if (client == -1 || client_addr_len != sizeof(client_addr)) {
		perror("failed to accept incoming connection");
		return;
	}
	printf("new connection: %s\n", inet_ntoa(client_addr.sin_addr));
	if (write(client, response, sizeof(response)) == -1) {
		printf("failed to send data\n");
	}
	close(client);
}
