#ifndef HTTP_H
#define HTTP_H

#include <stddef.h> // For size_t

#define BUFFER_SIZE 4096
#define WEB_ROOT "./www"

// Function Prototypes
void handle_client(int client_socket);
void send_response(int socket, int status_code, const char *status_message, const char *content_type, const char *body, size_t body_len);
void send_file(int socket, const char *filepath);
const char *get_mime_type(const char *filename);

#endif