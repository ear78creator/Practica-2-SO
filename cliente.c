#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int setup_socket() {
    return socket(AF_INET, SOCK_STREAM, 0);
}

void setup_server_address(struct sockaddr_in *serv_addr, const char *ip) {
    serv_addr->sin_family = AF_INET;
    serv_addr->sin_port = htons(9999);
    inet_pton(AF_INET, ip, &serv_addr->sin_addr);
}

void connect_to_server(int sock, struct sockaddr_in *serv_addr) {
    connect(sock, (struct sockaddr *)serv_addr, sizeof(*serv_addr));
}

void read_server_data(int sock) {
    char buffer[1024];
    int bytes_read;
    int reading = 1;

    while (reading == 1) {
        bytes_read = read(sock, buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            printf("%s", buffer);
        } else {
            reading = 0;
        }
    }
}

int main(int argc, char *argv[]) {
    int sock = setup_socket();
    struct sockaddr_in serv_addr;

    setup_server_address(&serv_addr, argv[1]);
    connect_to_server(sock, &serv_addr);
    read_server_data(sock);

    close(sock);
    return 0;
}
