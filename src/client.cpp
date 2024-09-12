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

#include "../include/server.h"
#include <unistd.h>
#include "../include/client.h"
#include <fstream>
#include <sstream>

TCP::ClientHandler::ClientHandler(int clientSocket,std::string clientIPAddr, std::string baseDirectory, logger& logger)
    :
    _clientSocket(clientSocket),
    _baseDirectory(std::move(baseDirectory)),
    _clientIpAdd(std::move(clientIPAddr)),
    _logger(logger)
{}

void
TCP::ClientHandler::handleRequest() const {

    _logger << "[" << _logger.getTimestamp() << "] Request from IP: " << _clientIpAdd << "\n";

    const std::string request = readRequest();
    if (request.empty()) {
        _logger << "[" << _logger.getTimestamp() << "] Error: Empty or malformed request." << "\n";
        send500();
        _logger.flushToFile();
        return;
    }

    const std::string method = extractMethod(request);
    const std::string filePath = extractFilePath(request);

    if (method == "GET") {
        if (filePath.empty()) {
            _logger << "[" << _logger.getTimestamp() << "] 404 Not Found: " << filePath << "\n";
            send404();
        } else {
            handleGetRequest(filePath);
        }
    } else if (method == "POST") {
        handlePostRequest(request, filePath);
    } else {
        _logger << "[" << _logger.getTimestamp() << "] 405 Method Not Allowed: " << method << "\n";
        send405();
    }

    _logger.flushToFile();  // Flush log at the end of request handling
}

void
TCP::ClientHandler::handleGetRequest(const std::string& filePath) const {
    std::ifstream file(filePath);

    if (!file) {
        _logger << "[" << _logger.getTimestamp() << "] 404 Not Found: " << filePath << "\n";
        send404();
        _logger.flushToFile();
        return;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    _logger << "[" << _logger.getTimestamp() << "] 200 OK: GET " << filePath << "\n";

    std::string response = "HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/html\r\n"
                           "Content-Length: " + std::to_string(content.size()) + "\r\n"
                           "\r\n" + content;
    send(_clientSocket, response.c_str(), response.size(), 0);
}

void
TCP::ClientHandler::handlePostRequest(const std::string& request, const std::string& filePath) const {
    const std::string postData = extractPostData(request);

    if (postData.empty()) {
        _logger << "[" << _logger.getTimestamp() << "] 400 Bad Request: POST " << filePath << "\n";
        send400();
        _logger.flushToFile();
        return;
    }

    _logger << "[" << _logger.getTimestamp() << "] 200 OK: POST " << filePath << "\n";

    const std::string response = "HTTP/1.1 200 OK\r\n"
                                 "Content-Type: text/plain\r\n"
                                 "Content-Length: " + std::to_string(postData.size()) + "\r\n"
                                 "\r\n" + postData;

    send(_clientSocket, response.c_str(), response.size(), 0);
}

// Utility methods

std::string
TCP::ClientHandler::extractPostData(const std::string& request) const {
    const size_t contentStart = request.find("\r\n\r\n");
    if (contentStart != std::string::npos) {
        return request.substr(contentStart + 4);  // Skip the headers and go to the body
    }
    return "";
}

std::string
TCP::ClientHandler::readRequest() const {
    char buffer[MY_CONSTANTS::BUFFER_SIZE] = {};

    if (const long bytesRead = read(_clientSocket, buffer, MY_CONSTANTS::BUFFER_SIZE); bytesRead < 0) {
        _logger << "Failed to read request from client." << "\n";
        _logger.flushToFile();
        return "";
    }

    return {buffer, MY_CONSTANTS::BUFFER_SIZE};
}

std::string
TCP::ClientHandler::extractMethod(const std::string& request) const {
    const auto firstSpace = request.find(' ');
    return request.substr(0, firstSpace);
}

std::string
TCP::ClientHandler::extractFilePath(const std::string& request) const {
    const auto firstSpace = request.find(' ');
    const auto secondSpace = request.find(' ', firstSpace + 1);
    std::string filePath = request.substr(firstSpace + 1, secondSpace - firstSpace - 1);

    if (filePath == "/") {
        filePath = "index.html";
    }

    return _baseDirectory + filePath;
}

// Status Code Methods

void
TCP::ClientHandler::send404() const {
    const std::string response = "HTTP/1.1 404 Not Found\r\n"
                                 "Content-Type: text/html\r\n"
                                 "\r\n"
                                 "<html><body><h1>404 Not Found</h1></body></html>";

    send(_clientSocket, response.c_str(), response.size(), 0);
}

void
TCP::ClientHandler::send405() const {
    const std::string response = "HTTP/1.1 405 Method Not Allowed\r\n"
                                 "Content-Type: text/html\r\n"
                                 "Allow: GET, POST\r\n"
                                 "\r\n"
                                 "<html><body><h1>405 Method Not Allowed</h1></body></html>";

    send(_clientSocket, response.c_str(), response.size(), 0);
}

void
TCP::ClientHandler::send500() const {
    const std::string response = "HTTP/1.1 500 Internal Server Error\r\n"
                                 "Content-Type: text/html\r\n"
                                 "\r\n"
                                 "<html><body><h1>500 Internal Server Error</h1></body></html>";

    send(_clientSocket, response.c_str(), response.size(), 0);
}

void
TCP::ClientHandler::send400() const {
    const std::string response = "HTTP/1.1 400 Bad Request\r\n"
                                 "Content-Type: text/html\r\n"
                                 "\r\n"
                                 "<html><body><h1>400 Bad Request</h1></body></html>";

    send(_clientSocket, response.c_str(), response.size(), 0);
}
