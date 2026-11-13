#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <fcntl.h>

int setup_socket() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    return server_fd;
}

void setup_address(struct sockaddr_in *address) {
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons(9999);
}

void bind_and_listen(int server_fd, struct sockaddr_in *address) {
    bind(server_fd, (struct sockaddr *)address, sizeof(*address));
    listen(server_fd, 5);
}

void transfer_data(int file_fd, int client_socket) {
    char buffer[1024];
    int bytes_read;
    int reading = 1;

    while (reading == 1) {
        bytes_read = read(file_fd, buffer, sizeof(buffer));
        if (bytes_read > 0) {
            write(client_socket, buffer, bytes_read);
        } else {
            reading = 0;
        }
    }
}

void process_client_request(int server_fd, int client_socket) {
    if (fork() == 0) {
        close(server_fd);
        int file_fd = open("Google.html", O_RDONLY);
        transfer_data(file_fd, client_socket);
        close(file_fd);
        close(client_socket);
        exit(0);
    }
}

void accept_connections(int server_fd, struct sockaddr_in *address) {
    int addrlen = sizeof(*address);
    int new_socket;
    int running = 1;

    while (running == 1) {
        new_socket = accept(server_fd, (struct sockaddr *)address, (socklen_t*)&addrlen);
        process_client_request(server_fd, new_socket);
        close(new_socket);
        waitpid(-1, NULL, WNOHANG);
    }
}

int main() {
    int server_fd = setup_socket();
    struct sockaddr_in address;

    setup_address(&address);
    bind_and_listen(server_fd, &address);
    accept_connections(server_fd, &address);

    return 0;
}
