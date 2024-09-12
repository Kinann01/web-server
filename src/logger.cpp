#include "../include/logger.h"

logger::logger(const std::string& filePath) {
    logFile.open(filePath, std::ios_base::app);
    if (!logFile.is_open()) {
        throw std::runtime_error("Failed to open log file: " + filePath);
    }
}

logger::~logger() {
    flushToFile();
    if (logFile.is_open()) {
        logFile.close();
    }
}

void
logger::logToFile(const std::string& message) {
    std::lock_guard<std::mutex> lock(mtx);
    logFile << message << std::endl;
    logFile.flush();
}

void
logger::logToBuffer(const std::string& message) {
    std::lock_guard<std::mutex> lock(mtx);
    buffer << message;
}

void
logger::flushToFile() {
    std::lock_guard<std::mutex> lock(mtx);
    if (!buffer.str().empty()) {  // Only write if there's something in the buffer
        logFile << buffer.str();  // Write buffer content to the log file
        logFile.flush();  // Ensure the content is written to disk
        buffer.str("");   // Clear the buffer content
        buffer.clear();   // Reset the stringstream state
    }
}

// Function to get the current timestamp
std::string
logger::getTimestamp() const {
    const auto now = std::chrono::system_clock::now();
    const std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    char buf[80];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now_c));
    return {buf};
}

logger&
logger::operator<<(const std::string& message) {
    logToBuffer(message);
    return *this;  // Return *this to allow chaining (e.g., logger << "log1" << "log2")
}
