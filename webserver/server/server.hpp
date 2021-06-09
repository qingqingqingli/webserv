/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: roybakker <roybakker@student.codam.nl>       +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/03/30 13:54:18 by roybakker     #+#    #+#                 */
/*   Updated: 2021/05/28 14:00:34 by gbouwen       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_SERVER_HPP
#define WEBSERV_SERVER_HPP

//general includes
#include <string>
#include <cstring>
#include <vector>
#include <map>
#include <list>
#include <iostream>
#include <unistd.h>
#include <exception>
#include <fcntl.h>

//custom includes
#include "client.hpp"
#include "location_context.hpp"
#include "header_handler.hpp"
#include "request_buf.hpp"

//tcp-connection includes
#include <netinet/in.h>
#include <sys/socket.h>

//custom color for better visibility
# define RESET			"\033[0m"
# define BLACK			"\033[30m"				/* Black */
# define RED			"\033[31m"				/* Red */
# define GREEN			"\033[32m"				/* Green */
# define YELLOW			"\033[33m"				/* Yellow */
# define BLUE			"\033[34m"				/* Blue */
# define MAGENTA		"\033[35m"				/* Magenta */
# define CYAN			"\033[36m"				/* Cyan */
# define WHITE			"\033[37m"				/* White */

class server {
public:
	friend class handler;
	friend class webserver;
    friend class file_descriptors;


public:
    typedef     std::vector<std::string>::iterator      vector_iterator;
    typedef     std::map<int, request_buf>::iterator    map_iterator;
    typedef     std::vector<client>::iterator           client_iterator;
    typedef     void (server::*configure)(const std::string&);
    enum        server_values{ port_ = 0, host_ = 1, server_name_ = 2, error_page_ = 3,
                                time_out_ = 4, cgi_file_types_ = 5, unknown_ = 6, location_ = 7 };

private:
    //Connection
    std::vector<client>             _clients;

    //Configurations
    int                             _port;
    int                             _time_out;
    std::string                     _host;
    std::string                     _server_name;
    std::string                     _error_page;
    std::string                     _cgi_file_types;
    std::vector<location_context>   _location_blocks;

    //TCP connection
    int 	                        _tcp_socket;
    int		                        _addr_len;
    struct	sockaddr_in             _addr;

    //Handler
    std::map<int, request_buf>      _request_buffer;

public:
    server();
    ~server();

    //GENERAL functions
    void    create_new_server(std::vector<std::string>& server_config);

    //CONFIG functions
    int     identify_server_value(const std::string& str);
    void    configure_port(const std::string& str);
    void    configure_host(const std::string& str);
    void    configure_server_name(const std::string& str);
    void    configure_time_out(const std::string& str);
    void    configure_cgi_file_types(const std::string& str);
    void    configure_error_page(const std::string& str);
    void    invalid_element(const std::string& str);

    //TCP functions
    void	create_socket();
    void	bind_socket_address(int port);
    void	create_connection(int backlog);

    //RESET functions
    void    reset_server();
    void    remove_handled_request(int used_fd);
    void    remove_client(int clientFD);

    //REQUEST functions
    int     update_request_buffer(int fd, const std::string& request_data);

    //GET functions
    int                             get_time_out();
    int                             get_port();
    std::string                     get_host();
    std::string                     get_server_name();
    std::string                     get_error_page();
    std::string                     get_cgi_file_types();
    std::vector<location_context>   get_location_blocks();
    int                             get_tcp_socket();
};

#endif //WEBSERV_SERVER_HPP
