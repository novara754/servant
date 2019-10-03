#define _GNU_SOURCE
#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/socket.h>

#define LISTEN_BACKLOG 50
#define REQUEST_BUFFER_LEN 1024
#define RESPONSE_BUFFER_LEN 1024

char request_buffer[REQUEST_BUFFER_LEN];
jmp_buf handle_client_error;

int create_server(uint16_t port);
void accept_client(int server_socket);

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		return 1;
	}

	char *end_of_port;
	uint16_t port = (uint16_t)strtoul(argv[1], &end_of_port, 10);
	if (end_of_port == argv[1]) {
		fprintf(stderr, "invalid port\n");
		return 1;
	}

	int server = create_server(port);
	printf("now accepting connections on port %d...\n", port);
	while (1) {
		accept_client(server);
	}

	return 0;
}

int create_server(uint16_t port) {
	int server = socket(AF_INET, SOCK_STREAM, 0);
	if (server < 0) {
		perror("failed to create socket");
		exit(1);
	}

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);

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

void http_error(int code, const char *msg) {
	fprintf(stderr, "%d - %s\n", code, msg);
	longjmp(handle_client_error, code);
}

void write_response(int fd, int code) {
	dprintf(
		fd,
		"HTTP/1.1 %d\n"
		"Content-Type: text/html\n"
		"\n"
		"<html>"
		"<head>"
			"<title>Error %d</title>"
		"</head>"
		"<body>"
			"<h1>Error %d</h1>"
		"</body>"
		"</html>",
		code, code, code
	);
}

void handle_client(int client_socket) {
	ssize_t num_read = read(client_socket, request_buffer, REQUEST_BUFFER_LEN);
	if (num_read == 0) {
		http_error(400, "EOF");
	}
	if (num_read < 0) {
		http_error(500, strerror(errno));
	}

	char *line = strtok(request_buffer, "\n");
	if (!line) {
		http_error(400, "empty status line");
	}

	char *method = strtok(line, " ");
	if (strcmp(method, "GET") != 0) {
		http_error(405, "unknown method");
	}

	char *path = strtok(NULL, " ");
	char filepath[PATH_MAX];
	getcwd(filepath, PATH_MAX);
	strcat(filepath, "/public");
	strncat(filepath, path, PATH_MAX);
	if (access(filepath, R_OK) != 0) {
		http_error(404, "unknown resource");
	}

	struct stat file_stat;
	stat(filepath, &file_stat);
	if ((file_stat.st_mode & S_IFREG) != S_IFREG) {
		http_error(404, "unknown resource");
	}

	char response[RESPONSE_BUFFER_LEN] =
		"HTTP/1.1 %d\n"
		"Content-Type: text/html\n"
		"\n";
	char file_content[RESPONSE_BUFFER_LEN];
	FILE *file = fopen(filepath, "r");
	fread(file_content, 1, RESPONSE_BUFFER_LEN, file);
	fclose(file);
	strncat(response, file_content, RESPONSE_BUFFER_LEN);
	if (write(client_socket, response, sizeof(response)) == -1) {
		printf("failed to send data\n");
	}
}

void accept_client(int server_socket) {
	struct sockaddr_in client_addr;
	socklen_t client_addr_len;
	int client = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
	if (client == -1 || client_addr_len != sizeof(client_addr)) {
		perror("failed to accept incoming connection");
		return;
	}


	pid_t pid = fork();
	if (pid == -1) {
		perror("failed to fork");
		return;
	}

	if (pid == 0) {
		printf("new connection: %s\n", inet_ntoa(client_addr.sin_addr));
		int code = setjmp(handle_client_error);
		if (code == 0) {
			handle_client(client);
		} else {
			write_response(client, code);
		}
		printf("--------------------\n");
	}

	close(client);
}
