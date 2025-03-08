#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>

#define PORT "2316"

/**
 * Returns the socket file descriptor the server is bound to if successful and -1 if not.
 */
int find_valid_socket(struct addrinfo *sockets, struct addrinfo *selected_socket) {
  int sockfd;
  int yes = 1;

  for (selected_socket = sockets; selected_socket != NULL; selected_socket = selected_socket->ai_next) {
    sockfd = socket(selected_socket->ai_family, selected_socket->ai_socktype, selected_socket->ai_protocol);

    if (sockfd == -1) {
      perror("socket");
      continue;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      perror("setsockopt");
      return -1;
    }

    if (bind(sockfd, selected_socket->ai_addr, selected_socket->ai_addrlen) == -1) {
      perror("bind");
      continue;
    }

    break;
  }

  freeaddrinfo(sockets);

  if (selected_socket == NULL) {
    fprintf(stderr, "server failed to bind");
    return -1;
  }

  return sockfd;
}

void parse_incoming_requests(int sockfd, struct addrinfo *selected_socket) {
  struct sockaddr_storage incoming_addr;
  socklen_t incoming_addr_size;

  printf("server waiting for connections on port %s...\n", PORT);

  while(1) {
    incoming_addr_size = sizeof incoming_addr;

    int incoming_sockfd = accept(sockfd, (struct sockaddr *)&incoming_addr, &incoming_addr_size);

    if (incoming_sockfd == -1) {
      perror("accept");
      continue;
    }

    if (!fork()) {
      close(sockfd);
      if (send(incoming_sockfd, "Hello, world!", 13, 0) == -1)
				perror("send");
			close(incoming_sockfd);
			exit(0);
    }

    close(incoming_sockfd);
  }
}

int main(void) {
  struct addrinfo hints;
  struct addrinfo *servinfo;
  struct addrinfo *selected_socket;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if (getaddrinfo(NULL, PORT, &hints, &servinfo) == -1) {
    perror("getaddrinfo");
    exit(1);
  }

  int sockfd = find_valid_socket(servinfo, selected_socket);

  if (sockfd == -1) {
    fprintf(stderr, "server could not find valid socket");
    exit(1);
  }

  if (listen(sockfd, 10) == -1) {
    perror("listen");
    exit(1);
  }

  parse_incoming_requests(sockfd, selected_socket);

  return 0;
}