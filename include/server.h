#ifndef SERVER_H
#define SERVER_H

#include <string>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    typedef SOCKET socket_t;
#else
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #include <unistd.h>
    typedef int socket_t;
#endif

#include "logger.h"

namespace MY_CONSTANTS {
    constexpr int BUFFER_SIZE = 30720;
}

namespace TCP {

    class HTTPServer {

    public:
        HTTPServer(std::string&& ip_addr, int p, std::string&& staticBaseDir, logger& myLogger);
        ~HTTPServer();
        bool startCommunication();

    private:
        bool createSocket();
        void closeListening();
        bool bindSocket();
        bool listenForConnections();

        std::string _ip_address;
        int _port;
        bool _isRunning;
        socket_t _socket;
        socket_t _new_socket;
        struct sockaddr_in _socketAddress;
        logger& _logger;

        std::string _staticBaseDir;
    };
}

#endif // SERVER_H
