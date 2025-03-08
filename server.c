#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

#define PORT "2316"

int main(void) {
  int status;
  struct addrinfo hints;
  struct addrinfo *servinfo;
  struct addrinfo *pointer;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  status = getaddrinfo(NULL, PORT, &hints, &servinfo);

  if (status == -1) {
    perror("getaddrinfo");
    exit(1);
  }

  int s;

  for (pointer = servinfo; pointer != NULL; pointer = pointer->ai_next) {
    s = socket(pointer->ai_family, pointer->ai_socktype, pointer->ai_protocol);

    if (s == -1) {
      perror("server: socket");
      continue;
    }

    int bind_status = bind(s, pointer->ai_addr, pointer->ai_addrlen);

    if (bind_status == -1) {
      perror("bind");
      continue;
    }

    break;
  }

  freeaddrinfo(servinfo);

  if (pointer == NULL) {
    fprintf(stderr, "server: failed to bind");
    exit(1);
  }

  int listen_status = listen(s, 10);

  if (listen_status == -1) {
    perror("listen");
    exit(1);
  }

  return 0;
}