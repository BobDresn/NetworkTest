#include <sys/socket.h>
#include <netinet/in.h>
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
*   Recv()
*/

void joinPlayer(std::queue<int>& openings, int players[], int client_fd)
{
    if(!openings.empty())
    {
       players[openings.front()] = client_fd;
       openings.pop();
    } else {
        std::cout << "Error joining\n";
    }
}



int main() 
{
    //Local server file descriptor
    int server_fd, client_fd = 0;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    //Create server socket
    if( server_fd != 0 ) 
    {
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
    server_addr.sin_port = 0;

    //Bind socket to ip and port
    if( bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0 ) 
    {
        std::cout << "Bind failed.\n";
        return 1;
    }

    int port = server_addr.sin_port;
    std::cout << "Server port is: " << port << std::endl;

    //Sets thread to listen for connections
    if( listen(server_fd, 5) < 0 )
    {
        std::cout << "Listen failed\n";
        return 1;
    }

    //Game Structures
    int players[5] = {0, 0, 0, 0, 0};
    std::queue<int> openings;


    //Sets the queue to 
    for(int i = 0; i < 5; i++)
    {
        openings.push(i);
    }

    //GAME LOOP
    /*
    This will run the game on the server. Server holds an array of players with their data, and a queue tracks open spots. 
    */
    while(true)
    {
        //Adds new players to the server
        if((client_fd = accept(server_fd, (struct sockaddr*)&server_addr, (socklen_t*)sizeof(server_addr))) != 0)
        {
            //Handles failures or no connections
            std::cout << "Nothing to accept\n";
        } else {
            //Joins successful connection
            joinPlayer(openings, players, client_fd);
            std::cout << "Player connected!\n" << client_fd << std::endl;
        }

        std::cin.get();
        // std::cout << "Continue? (y/n) ";
        // char in = ' ';
        // std::cin >> in;
        // if(in != 'y')
        // {
        //     close(server_fd);
        //     return 1;
        // }
    }
    return 0;
}