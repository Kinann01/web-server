#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include "logger.h"

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

namespace TCP {

    class ClientHandler {

    public:
        ClientHandler(socket_t clientSocket, std::string clientIPAddr, std::string baseDirectory, logger& logger);
        void handleRequest() const;

    private:
        [[nodiscard]] std::string readRequest() const;

        void handlePostRequest(const std::string &request, const std::string& filePath) const;
        void handleGetRequest(const std::string& filePath) const;

        [[nodiscard]] std::string extractMethod(const std::string& request) const;
        [[nodiscard]] std::string extractFilePath(const std::string& request) const;
        [[nodiscard]] std::string extractPostData(const std::string &request) const;

        void send404() const;
        void send500() const;
        void send405() const;
        void send400() const;

        socket_t _clientSocket;
        std::string _baseDirectory;
        std::string _clientIpAdd;
        logger& _logger;
    };
}

#endif // CLIENT_H
