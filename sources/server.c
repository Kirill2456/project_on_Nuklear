/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void error_exit(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main() {
    WSADATA wsaData;
    SOCKET server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    int addr_len = sizeof(client_addr);
    float numbers[2];
    float result_array[BUFFER_SIZE];

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        error_exit("WSAStartup failed");
    }

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        WSACleanup();
        error_exit("Socket creation failed");
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        closesocket(server_fd);
        WSACleanup();
        error_exit("Bind failed");
    }

    if (listen(server_fd, 5) == SOCKET_ERROR) {
        closesocket(server_fd);
        WSACleanup();
        error_exit("Listen failed");
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_fd == INVALID_SOCKET) {
            printf("Failed to accept connection\n");
            continue;
        }

        int bytes_received = recv(client_fd, (char*)numbers, sizeof(numbers), 0);
        if (bytes_received == sizeof(numbers)) {
            //тут должно быть заполнение дистанцией
            float sum = numbers[0] + numbers[1];
            for (int i = 0; i < BUFFER_SIZE; i++) {
                result_array[i] = sum;
            }

            send(client_fd, (char*)result_array, sizeof(result_array), 0);
        } else {
            printf("Failed to receive data from client\n");
        }

        closesocket(client_fd);
    }

    closesocket(server_fd);
    WSACleanup();
    return 0;
}
*/