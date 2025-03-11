#ifndef HTTP_H_
#define HTTP_H_

/**
 * Returns status code of request. Response message will be stored in response.
 */
int parse_request(char *type, char *path, char *protocol, char **response);

#endif