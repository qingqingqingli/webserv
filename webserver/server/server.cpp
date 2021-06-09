/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   server.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: roybakker <roybakker@student.codam.nl>       +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/03/30 13:54:06 by roybakker     #+#    #+#                 */
/*   Updated: 2021/05/18 14:12:46 by gbouwen       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"
#include "../helper/helper.hpp"

server::server() : _clients(), _port(0), _host(), _server_name(), _error_page(), _cgi_file_types(), _location_blocks(), _tcp_socket(0),
				   _addr_len(0), _addr(), _request_buffer(){}
server::~server(){}


//-------------------------------------- GENERAL functions --------------------------------------
void    server::create_new_server(std::vector <std::string>& server_config) {
    location_context                location;
    configure configure_array[7] = { &server::configure_port,
                                     &server::configure_host,
                                     &server::configure_server_name,
                                     &server::configure_error_page,
                                     &server::configure_time_out,
                                     &server::configure_cgi_file_types,
                                     &server::invalid_element };

    reset_server(); // CAN BE REMOVED?
    for (vector_iterator it = server_config.begin(); it != server_config.end(); it++) {
        int server_value = identify_server_value(*it);
        if (server_value == location_) {
			location.configure_location_block(it, (it + location_block_size(it, server_config.end())));
            _location_blocks.push_back(location);
        }
        else {
            configure function = configure_array[server_value];
            (this->*function)(*it);
        }
    }
}


//-------------------------------------- CONFIG functions --------------------------------------
int     server::identify_server_value(const std::string& str) {
    if (str.find("port") != std::string::npos)
        return port_;
    else if (str.find("host") != std::string::npos)
        return host_;
    else if (str.find("server_name") != std::string::npos)
        return server_name_;
    else if (str.find("error_page") != std::string::npos)
        return error_page_;
    else if (str.find("time_out") != std::string::npos)
        return time_out_;
    else if (str.find("cgi_file_types") != std::string::npos)
        return cgi_file_types_;
    else if (str.find("location") != std::string::npos)
        return location_;
    return unknown_;
}

void    server::configure_port(const std::string& str) {_port = parse_number(str);}
void    server::configure_host(const std::string& str) {_host = parse_string(str);}
void    server::configure_server_name(const std::string& str) {_server_name = parse_string(str);}
void    server::configure_error_page(const std::string& str) {_error_page = parse_string(str);}
void    server::configure_time_out(const std::string& str) {_time_out = parse_number(str);}
void    server::configure_cgi_file_types(const std::string& str) {_cgi_file_types = parse_string(str);}
void    server::invalid_element(const std::string& str) {parse_invalid(str);}


//-------------------------------------- TCP functions --------------------------------------
void	server::create_socket() {
    if ((this->_tcp_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		throw (std::runtime_error("Socket creation failed"));
    if (fcntl(this->_tcp_socket, F_SETFL, O_NONBLOCK) == -1)
		throw (std::runtime_error("FCNTL failed"));
}

void	server::bind_socket_address(int port) {
    memset((char *)&_addr, 0, sizeof(_addr));
    _addr.sin_family = AF_INET;
    _addr.sin_addr.s_addr = htonl(INADDR_ANY);
    _addr.sin_port = htons(port);

    if (bind(this->_tcp_socket, (struct sockaddr *)&_addr, sizeof(_addr)) == -1)
		throw (std::runtime_error("Bind failed"));
}

void	server::create_connection(int backlog) {
    if (listen(this->_tcp_socket, backlog) == -1)
		throw (std::runtime_error("Listen failed"));
}


//-------------------------------------- RESET functions --------------------------------------
void    server::reset_server(){
    _port = 0;
    _time_out = 0;
    _host.clear();
    _server_name.clear();
    _error_page.clear();
    _cgi_file_types.clear();
    _location_blocks.clear();
}

void    server::remove_handled_request(int used_fd){
    map_iterator request = _request_buffer.find(used_fd);

    request->second.get_body().clear();
    request->second.set_body_size(0);
    _request_buffer.erase(request);
}

void    server::remove_client(int clientFD) {
    for (client_iterator it = _clients.begin(); it != _clients.end(); it++) {
        int client = it->get_clientFD();
        if (clientFD == client) {
            close(client);
            _clients.erase(it);
        }
    }
}


//-------------------------------------- REQUEST functions --------------------------------------
int     server::update_request_buffer(int fd, const std::string& request_data) {
	std::map<int, request_buf>::iterator it = _request_buffer.find(fd);

    if (it == _request_buffer.end()) {
        request_buf tmp;
        _request_buffer.insert(std::pair<int, request_buf>(fd, tmp));
    }

    if (!_request_buffer[fd].headers_received()) {
		_request_buffer[fd].add_header_data(request_data);
	}
    else {
		_request_buffer[fd].add_body_data(request_data);
	}

    if (!_request_buffer[fd].headers_received())
        return invalid_;
    else
        return _request_buffer[fd].validate_request();
}


//-------------------------------------- GET functions --------------------------------------
int                             server::get_time_out() {return _time_out;}
int                             server::get_port() {return _port;}
std::string                     server::get_host() {return _host;}
std::string                     server::get_server_name() {return _server_name;}
std::string                     server::get_error_page() {return _error_page;}
std::string                     server::get_cgi_file_types() {return _cgi_file_types;}
std::vector<location_context>   server::get_location_blocks() {return _location_blocks;}
int                             server::get_tcp_socket() {return _tcp_socket;}
