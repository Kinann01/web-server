#include "../include/server.h"
#include "../include/logger.h"
#include <fstream>
#include <iostream>


int main() {

    std::string filePath = "../config.txt";
    std::fstream fs(filePath, std::fstream::in);

    if (!fs.is_open()) {
        std::cerr << "Failed to open file " << filePath << std::endl;
        return 1;
    }

    int portNumber = -1;
    fs >> portNumber;
    std::string ipaddr = {};
    fs >> ipaddr;
    std::string dir = {};
    fs >> dir;
    std::string loggerFile = {};
    fs >> loggerFile;

    auto myLogger = logger(loggerFile);

    auto server = TCP::HTTPServer(
        std::move(ipaddr),
        portNumber,
        std::move(dir),
        myLogger);

    if (auto success = server.startCommunication(); !success) {
        std::cout<<"Refer to log File"<<std::endl;
    }

    return 0;
}
