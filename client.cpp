#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>

/*
Socket()
Connect()
Send()
Recv()
?
*/
int main() 
{
    //Create Socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(clientSocket == -1) {
        std::cout << "Failed creating socket.\n";
        return 1;
    }

    //TODO- GET INPUT FOR IP ADDRESS, WILL BE USED FOR ACCESSING OTHER HOSTS LATER
    //specify address
    struct sockaddr_in server_addr;
    server_addr.sin_addr.s_addr = htons(23512);
    //Sets sockaddr_in parameters
    server_addr.sin_family = AF_INET;
    //std::cout << "Please enter server port: ";
    int input = 0;
    //std::cin >> input;
    server_addr.sin_port = htons(42069);
    //Parses IP address into sin_addr for parent struct
    std::string address = "127.0.0.1";
    if(inet_aton(address.c_str(), &server_addr.sin_addr) == -1)
    {
        std::cout << "Failed parsing IP.\n";
        return 1;
    }

    //Connect() automatically binds to a port if not done already. Redundant if added twice
    while( connect(clientSocket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::cout << "Failed connecting\n";
        return 1;
    }

    std::cout << "Connection made at: " << clientSocket << std::endl;

    while(true) {
        char input[1024];
        std::cout << "Enter your message: ";
        std::cin >> input;
        const char* message = input;
        send(clientSocket, message, strlen(message), 0);
    }
    return 0;
}