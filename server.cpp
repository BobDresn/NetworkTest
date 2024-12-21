#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <thread>
#include <functional>
/*
*   Socket()
*   Bind()
*   Listen()
*   Accept()
*   Send()
*   Read()
*/

struct PlayerPosition {
public: 
    int _x;
    int _y;
};

class Player {
public:
    PlayerPosition _pos;
    int _fd;

    Player() {
        this->_pos._x = 0;
        this->_pos._y = 0;
        this->_fd = 0;
    }

    Player(int x, int y, int fd) {
        this->_pos._x = x;
        this->_pos._y = y;
        this->_fd = fd;
    }
};

void activePlayer(int client_fd, std::queue<int>& available, Player*  players) {
    //Stack buffer for messages
    char buffer[8] = {0};
    int index = -1;
    for(int i = 0; i < 10; i++) {
        if(client_fd == players[i]._fd) {
            index = i;
            break;
        }
    }

    while(true) {
        //Handles disconnects first
        if(read(client_fd, buffer, sizeof(buffer)) <= 0) {
            close(client_fd);//Close client fd
            available.push(index);//Adds index to available queue
            players[index] = Player();//Clears player info
            break;
        } else {
            //Reads from buffer, parsing new location
            std::memcpy(&players[index]._pos._x, buffer, 4);
            std::memcpy(&players[index]._pos._y, buffer + 4, 4);
        }
        //Sends all player info
        send(players[index]._fd, players, sizeof(players), 0);
        //Starts sending updates of other players and receiving updates of current thread
    } 
}



int main() {
    //Local server file descriptor
    int server_fd, client_fd = -1;
    int port = 42069;

    //Creates socket file descriptor, unique int id on specific port & IP
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if( server_fd == -1 ) {
        std::cout << "Socket creation failed.\n";
        return 1;
    }

    //sockaddr local, client, and temporary
    struct sockaddr_in server_addr, client_addr;

   //SERVER
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    //Sets server to local host
    if(inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) == -1) {
        std::cout << "Invalid address\n";
        close(server_fd);
        return 1;
    }

    //Ensures succesful binding of socket to ip and port
    if( bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1 ) {
        std::cout << "Bind failed.\n";
        close(server_fd);
        return 1;
    }

    //Outputs hosted port. Useful to communicate to external connections
    port = server_addr.sin_port;
    std::cout << "Server hosted on is: " << ntohs(server_addr.sin_port) << std::endl;

    //players holds client_fd numbers of active connections
    //available keeps track of openings in players
    //clientThreads keeps track of all active threads
    Player players[10];
    std::queue<int> available;
    std::vector<std::thread> clientThreads;

    //Loads all slots of players into queue on start
    for(int i = 0; i < 10; i++) {
        available.push(i);
    }

    //Tells kernel to listen to this port for connections
    if(listen(server_fd, 10) == -1) {
        std::cout << "Listening failed.\n";
        close(server_fd);
        return 1;
    }

    //When accepting the socklen_t * in accept, needs to be a pointer to the size, has to be loaded as a precalculated variable
    socklen_t client_len = sizeof(client_addr);

    /////////////////////////////////////////////////////////////////////////////
    ///////START CONNECTIONS/////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////

    while(true) {
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if(client_fd != -1) { //Accept player
            players[available.front()]._fd = client_fd;//Place players fd in array of active connections
            //Create thread handling new players connection
            clientThreads.emplace_back([client_fd, &available, &players]() mutable {
                activePlayer(client_fd, available, players);
            });
            //Remove player slot from queue of available
            available.pop();
        }
    }

    //TODO Create new thread to handle messages for each connection. i.e. Each user connected is handled concurrently
    // do {
    //     client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
    //     if(client_fd != -1) {
    //         std::cout << "Player connected!\n" << client_fd << std::endl;
    //     }   
    // } while(client_fd == -1);

    while(true) {
        //If connections are open
        Player buffer[10] = {Player()};
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