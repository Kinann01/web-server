#include "../include/server.h"
#include <string>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "Ws2_32.lib")
    typedef SOCKET socket_t;
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    typedef int socket_t;
#endif

#include "../include/client.h"
#include "../include/threadpool.h"

TCP::HTTPServer::HTTPServer(std::string&& ip_addr, int p, std::string&& staticBaseDir, logger& myLogger)
    : _ip_address(std::move(ip_addr)),
    _port(p),
    _isRunning(false),
    _socket(-1),
    _new_socket(-1),
    _socketAddress({}),
    _logger(myLogger),
    _staticBaseDir(staticBaseDir)
{
#ifdef _WIN32
    // Initialize Winsock
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        _logger.logToFile("WSAStartup failed with error: " + std::to_string(result));
    }
#endif
}

bool
TCP::HTTPServer::startCommunication() {
    if (!createSocket() || !bindSocket() || !listenForConnections()) {
        return false; // create a socket, bind it, and start listening
    }

    _logger.logToFile("Server started listening on " + _ip_address + ":" + std::to_string(_port));

    auto myThreadPool = Thread_pool_t();

    while (_isRunning) {
        sockaddr_in client{};
        socklen_t clientSize = sizeof(client); // client connection comes
        _new_socket = accept(_socket, reinterpret_cast<sockaddr *>(&client), &clientSize); // accept the connection

        if (_new_socket == -1) {
#ifdef _WIN32
            int errorCode = WSAGetLastError();
            _logger.logToFile("Error accepting connection: " + std::to_string(errorCode));
#else
            _logger.logToFile("Error accepting connection");
#endif
            continue;
        }

        // Get the client's IP address
        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client.sin_addr, clientIP, INET_ADDRSTRLEN);

        // Start handling the request
        auto clientHandler = std::make_shared<ClientHandler>(_new_socket,
            std::string(clientIP), _staticBaseDir, _logger);

        myThreadPool.enqueue([clientHandler, this]() {
            clientHandler->handleRequest();
#ifdef _WIN32
            closesocket(_new_socket);
#else
            close(_new_socket);
#endif
        });
    }

    return true;
}

bool
TCP::HTTPServer::createSocket() {
    _socket = socket(AF_INET, SOCK_STREAM, 0);

    if (_socket == -1) {
#ifdef _WIN32
        int errorCode = WSAGetLastError();
        _logger.logToFile("Failed to create socket: " + std::to_string(errorCode));
#else
        _logger.logToFile("Failed to create socket");
#endif
        return false;
    }

    return true;
}

bool
TCP::HTTPServer::bindSocket() {
    _socketAddress.sin_family = AF_INET;
    _socketAddress.sin_addr.s_addr = inet_addr(_ip_address.c_str());
    _socketAddress.sin_port = htons(_port);

    if (bind(_socket, reinterpret_cast<sockaddr*>(&_socketAddress), sizeof(_socketAddress)) < 0) {
#ifdef _WIN32
        int errorCode = WSAGetLastError();
        _logger.logToFile("Failed to bind socket: " + std::to_string(errorCode));
#else
        _logger.logToFile("Failed to bind socket");
#endif
        return false;
    }

    return true;
}

bool
TCP::HTTPServer::listenForConnections() {
    if (listen(_socket, 20) < 0) {
#ifdef _WIN32
        int errorCode = WSAGetLastError();
        _logger.logToFile("Failed to listen on socket: " + std::to_string(errorCode));
#else
        _logger.logToFile("Failed to listen on socket");
#endif
        return false;
    }

    _isRunning = true;
    return true;
}

void
TCP::HTTPServer::closeListening() {
    if (_socket != -1) {
#ifdef _WIN32
        closesocket(_socket);
#else
        close(_socket);
#endif
        _socket = -1;
    }
    if (_new_socket != -1) {
#ifdef _WIN32
        closesocket(_new_socket);
#else
        close(_new_socket);
#endif
        _new_socket = -1;
    }
    _isRunning = false;
    _logger.logToFile("[" + _logger.getTimestamp() + "] Server Stopped");

#ifdef _WIN32
    WSACleanup();
#endif
}

TCP::HTTPServer::~HTTPServer() {
    closeListening();
}