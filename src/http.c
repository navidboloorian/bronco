#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "http.h"

int get(char *path, char **response) {
    path++;

    if (strcmp(path, "") == 0) {
      path = "index.html\0";
    }

    char header[100];
    int status_code = 0;

    FILE *file = fopen(path, "r"); 
    char *file_contents;

    if (file == NULL) {
      strcpy(header, "HTTP/1.1 404 Not Found\n");
      file = fopen("404.html", "r");
      status_code = 404;
    } else {
      strcpy(header, "HTTP/1.1 200 OK\n");
      status_code = 200;
    }

    fseek(file, 0L, SEEK_END);
    long size = ftell(file);
    rewind(file);

    file_contents = (char *)malloc(size);
    
    fread(file_contents, 1, size, file);
    fclose(file);

    int content_length = strlen(file_contents);
    char content_length_header[100];

    sprintf(content_length_header, "Content-Length: %d\nConnection: close\n\n", content_length);

    *response = (char *)malloc(strlen(header) + strlen(file_contents) + strlen(content_length_header));
    sprintf(*response, "%s%s%s", header, content_length_header, file_contents);

    free(file_contents);

    return status_code;
}

void parse_request(char *type, char *path, char *protocol, char **response, int sockfd) {
  if (strcmp(type, "GET") == 0) {
    int status_code = get(path, response);

    send(sockfd, *response, strlen(*response), 0);

    printf("%s %s: %d\n", type, path, status_code);
    free(*response);
  }
}