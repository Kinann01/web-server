# Web Server
## Introduction

The following repository contains a cross-platform multithreaded web server that uses socket programming. The server is capable of handling multiple requests from clients concurrently that is managed by a custom thread pool. Basic HTTP methods like GET and POST are supported. There is also a suite of tests written in python and wrapped in CMake to test the server.

## Dependencies

The following dependencies are required to run the web server:
- CMake: Build system
- Make: Build system
- A compiler that supports at least c++17
- Testing dependencies:
  - Python3
  - Pip3
  - requirements.txt

## Usage

- To build and run the web server on Linux, run the following commands:
```bash
mkdir build && cd build
cmake ..
make
./src/web-server
```
To test the web server on Linux, run the following commands:
```bash
cd build && cd build
cmake ..
make
ctest --verbose
```

- To build and run the web server on Windows, first open the Developer Command Prompt for Visual Studio and run the following commands:
```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
cd src
web-server.exe
```

To test the web server on Windows, first open the Developer Command Prompt for Visual Studio and run the following commands:
```bash
cd build && cd build
cmake ..
cmake --build . --config Release
ctest --verbose
```

It is also recommended to use a python virtual environment to run the tests instead of downloading the dependencies directly. To create a virtual environment, run the following commands:
```bash
python3 -m venv venv
source venv/bin/activate
pip3 install -r requirements.txt
```

Before building the server, ensure that the `config.txt` contains the expected configuration. The config.txt file is located in the root of the repository. The configuration file contains the following parameters:
- PORT: The port number the server listens on
- IP: The IP address the server listens on
- Static: The directory where the static files are located
- Logging: The file where the log file is located/Created

## Implementation Details

The web server is implemented using the following classes:
- `Server`: The main class that manages the server and listens for incoming connections.
- `ThreadPool`: A custom thread pool that manages the threads to handle the incoming requests.
- `Client` : Manages the client connection and processes the incoming requests.
- `logger` : A custom logger class that logs the requests and responses to a file


## Testing

The tests are written in python using PyTest and utilizes the requests library to send requests to the server. The tests are wrapped in CMake to run them using the CTest framework. The tests are located in the `tests` directory.

## Logging 

Logging is done using a custom logger class that logs the requests and responses to a file. The log file located in the root of the repository is named `server.log`.