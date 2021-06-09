
cmake_minimum_required(VERSION 3.17)

project(webserver)

set(SOURCES webserver/main.cpp
        webserver/gnl/get_next_line.cpp
        webserver/gnl/get_next_line_utils.cpp
        webserver/server/location_context.cpp
        webserver/server/file_descriptors.cpp
        webserver/server/response.cpp
        webserver/server/server.cpp
        webserver/server/webserver.cpp
        webserver/server/header_handler.cpp
        webserver/server/request_buf.cpp
        webserver/server/client.cpp
        webserver/helper/check_functions.cpp
        webserver/helper/lib_functions.cpp
        webserver/helper/parse_functions.cpp
        webserver/helper/signal_handler.cpp
        webserver/helper/verify_file_location_functions.cpp
        webserver/server/Base64.cpp)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

add_executable(webserver ${SOURCES})