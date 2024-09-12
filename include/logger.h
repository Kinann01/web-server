#ifndef LOGGER_H
#define LOGGER_H

#include <mutex>
#include <fstream>
#include <sstream>
#include <string>

class logger {

public:
    explicit logger(const std::string& filePath);
    ~logger();

    void logToBuffer(const std::string& message);  // Add a log message to the buffer
    void flushToFile();  // Write the buffer to file and clear it
    std::string getTimestamp() const;
    void logToFile(const std::string& message);
    logger& operator<<(const std::string& message);


private:
    std::mutex mtx;  // Protect concurrent access to logger
    std::ofstream logFile;  // The log file stream
    std::stringstream buffer;  // Buffer to accumulate log messages
};

#endif //LOGGER_H
