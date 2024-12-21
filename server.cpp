#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include <queue>

/*
*   Socket()
*   Bind()
*   Listen()
*   Accept()
*   Send()
*   Read()
*/

int main() {
    //Local server file descriptor
    int server_fd, client_fd = -1;
    int port = 42069;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    //Create server socket
    if( server_fd == -1 ) {
        std::cout << "Socket creation failed.\n";
        return 1;
    }

    //sockaddr local, client, and temporary
    struct sockaddr_in server_addr, client_addr;

    /*
    Family will always be AF_INET, handles IPV4 domain sockets
    INADDR_ANY binds any 
    Port @ 0 is supposed to tell OS to give a random unregistered port- not working?
    */

   //SERVER
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if(inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) == -1) {
        std::cout << "Invalid address\n";
        close(server_fd);
        return 1;
    }

    //Bind socket to ip and port
    if( bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1 ) {
        std::cout << "Bind failed.\n";
        close(server_fd);
        return 1;
    }

    port = server_addr.sin_port;
    std::cout << "Server hosted on is: " << ntohs(server_addr.sin_port) << std::endl;

    //Tells kernel to listen to this port for connections
    if(listen(server_fd, 5) == -1) {
        std::cout << "Listening failed.\n";
        close(server_fd);
        return 1;
    }
    
    if(client_fd != -1) {
        //Joins successful connection
        std::cout << "Player connected!\n" << client_fd << std::endl;
    }

    socklen_t client_len = sizeof(client_addr);

    do {
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if(client_fd != -1) {
        //Joins successful connection
        //joinPlayer(openings, players, client_fd);
        std::cout << "Player connected!\n" << client_fd << std::endl;
        }   
    } while(client_fd == -1);

    while(true) {
        char buffer[1024] = {0};
        if(read(client_fd, buffer, sizeof(buffer)) <= 0) {
            close(client_fd);
            close(server_fd);
            return -1;
        } 

        std::cout << buffer << std::endl;
    }
    
    close(client_fd);
    close(server_fd);
    return 0;
}