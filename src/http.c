#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
      strcpy(header, "HTTP/1.1 404 Not Found\n\n\0");
      file = fopen("404.html", "r");
      status_code = 404;
    }
    else {
      strcpy(header, "HTTP/1.1 200 OK\n\n\0");
      status_code = 200;
    }

    fseek(file, 0L, SEEK_END);
    long size = ftell(file);
    rewind(file);

    file_contents = (char *)malloc(size);

    fread(file_contents, 1, size, file);
    fclose(file);

    *response = (char *)malloc(strlen(header) + strlen(file_contents));
    sprintf(*response, "%s%s", header, file_contents);

    free(file_contents);

    return status_code;
}

int parse_request(char *type, char *path, char *protocol, char **response) {
  if (strcmp(type, "GET") == 0) {
    return get(path, response);
  }

  return 0;
}