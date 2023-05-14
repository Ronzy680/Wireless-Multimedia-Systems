#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8000
#define BUFFER_SIZE 1024

int main() {
    // Create a socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Define the server address
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    // Bind the socket to the address and port
    if (bind(server_socket, (sockaddr*) &server_address, sizeof(server_address)) < 0) {
        std::cerr << "Error binding socket\n";
        return 1;
    }

    // Listen for incoming connections
    listen(server_socket, 1);
    std::cout << "Server listening on port " << PORT << "\n";

    // Accept a client connection
    sockaddr_in client_address;
    socklen_t client_address_size = sizeof(client_address);
    int client_socket = accept(server_socket, (sockaddr*) &client_address, &client_address_size);
    if (client_socket < 0) {
        std::cerr << "Error accepting connection\n";
        return 1;
    }

    // Open the multimedia file to be sent
    std::ifstream file("example.mp4", std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Error opening file\n";
        return 1;
    }
    std::streampos file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    // Send the multimedia file to the client
    char buffer[BUFFER_SIZE];
    std::streampos bytes_sent = 0;
    while (bytes_sent < file_size) {
        int bytes_to_send = std::min(BUFFER_SIZE, static_cast<int>(file_size - bytes_sent));
        file.read(buffer, bytes_to_send);
        int bytes_sent_now = send(client_socket, buffer, bytes_to_send, 0);
        if (bytes_sent_now < 0) {
            std::cerr << "Error sending data\n";
            return 1;
        }
        bytes_sent += bytes_sent_now;
    }

    // Close the socket and file
    close(client_socket);
    close(server_socket);
    file.close();

    return 0;
}
