#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>

int main() 
{
    //Create Socket
    int local_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(local_fd == -1)
    {
        std::cout << "Failed creating socket.\n";
        return 1;
    }

    //Create Sockaddr_in. Creates Ip. 
    //TODO- GET INPUT FOR IP ADDRESS, WILL BE USED FOR ACCESSING OTHER HOSTS LATER
    struct sockaddr_in server_addr;
    std::string address = "127.0.0.1";
    //Parses IP address into sin_addr for parent struct
    if(inet_aton(address.c_str(), &server_addr.sin_addr) < 0)
    {
        std::cout << "Failed parsing IP.\n";
        return 1;
    }
    //Sets sockaddr_in parameters
    std::cout << "Please enter server port: ";
    int input = 0;
    std::cin >> input;
    server_addr.sin_port = htons(input);
    server_addr.sin_family = AF_INET;

    //Connect() automatically binds to a port if not done already. Redundant if added twice
    if( connect(local_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
    {
        std::cout << "Failed connecting\n";
        return 1;
    }

    // if( connect(server_addr, ) == -1)
    // {
    //     std::cout << "Failed to connect.\n";
    //     return 1;
    // }
}