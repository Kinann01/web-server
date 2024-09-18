add_executable(web-server main.cpp server.cpp logger.cpp threadpool.cpp client.cpp)
target_include_directories(web-server PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_options(web-server PRIVATE -Wall -Wextra)

if(WIN32)
    message(STATUS "Building on Windows")
    target_link_libraries(web-server wsock32 ws2_32)
    target_compile_definitions(web-server PRIVATE _WIN32_WINNT=0x0601)
    
    # Version Codes for _WIN32_WINNT:
    # 0x0501: Windows XP
    # 0x0502: Windows Server 2003
    # 0x0600: Windows Vista
    # 0x0601: Windows 7
    # 0x0602: Windows 8
    # 0x0603: Windows 8.1
    # 0x0A00: Windows 10

elseif(APPLE)
    message(STATUS "Building on macOS")
elseif(UNIX)
    message(STATUS "Building on a Unix-like system (Linux, etc.)")
    target_link_libraries(web-server pthread)
endif()
