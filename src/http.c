#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "../include/http.h"

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE] = {0};
    
    ssize_t bytes_read = read(client_socket, buffer, BUFFER_SIZE - 1);
    if (bytes_read <= 0) {
        close(client_socket);
        return;
    }

    char method[16], path[256], protocol[16];
    sscanf(buffer, "%s %s %s", method, path, protocol);

    printf("[REQ] %s %s\n", method, path);

    if (strcmp(method, "GET") != 0) {
        const char *body = "Method Not Allowed";
        send_response(client_socket, 405, "Method Not Allowed", "text/plain", body, strlen(body));
    } else {
        if (strcmp(path, "/") == 0) {
            strcpy(path, "/index.html");
        }

        char filepath[512];
        snprintf(filepath, sizeof(filepath), "%s%s", WEB_ROOT, path);

        send_file(client_socket, filepath);
    }

    close(client_socket);
}

void send_file(int socket, const char *filepath) {
    int file_fd = open(filepath, O_RDONLY);
    if (file_fd == -1) {
        const char *body = "404 Not Found";
        send_response(socket, 404, "Not Found", "text/plain", body, strlen(body));
        return;
    }

    struct stat file_stat;
    fstat(file_fd, &file_stat);
    size_t file_size = file_stat.st_size;

    char *file_buffer = malloc(file_size);
    read(file_fd, file_buffer, file_size);

    send_response(socket, 200, "OK", get_mime_type(filepath), file_buffer, file_size);

    free(file_buffer);
    close(file_fd);
}

void send_response(int socket, int status_code, const char *status_message, const char *content_type, const char *body, size_t body_len) {
    char header[BUFFER_SIZE];
    
    int header_len = snprintf(header, BUFFER_SIZE,
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "Server: Custom-C-Server/1.0\r\n"
        "\r\n",
        status_code, status_message, content_type, body_len
    );

    write(socket, header, header_len);
    if (body_len > 0) {
        write(socket, body, body_len);
    }
}

const char *get_mime_type(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if (!dot) return "text/plain";
    if (strcmp(dot, ".html") == 0) return "text/html";
    if (strcmp(dot, ".css") == 0) return "text/css";
    if (strcmp(dot, ".js") == 0) return "application/javascript";
    if (strcmp(dot, ".png") == 0) return "image/png";
    if (strcmp(dot, ".jpg") == 0) return "image/jpeg";
    return "text/plain";
}