/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   webserver.cpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: roybakker <roybakker@student.codam.nl>       +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/03/30 16:30:47 by roybakker     #+#    #+#                 */
/*   Updated: 2021/05/28 13:55:59 by gbouwen       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../gnl/get_next_line.hpp"
#include "../helper/helper.hpp"
#include "webserver.hpp"
#include "server.hpp"

webserver::webserver() : _servers(), _time_out_check(false) {}
webserver::~webserver(){}


//-------------------------------------- GENERAL functions --------------------------------------
void    webserver::load_configuration(char *config_file) {
    std::vector<std::string>    server_block;
    server                      server;
    char                        *line = NULL;
    int                         ret = 0;
    int                         fd = open(config_file, O_RDONLY);

	if (fd == -1)
		throw std::invalid_argument("Error opening config file");
    while ((ret = get_next_line(fd, &line)) == 1) {
        server_block.push_back(line);
        free(line);
        if (check_server_block(server_block)) {
            server.create_new_server(server_block);
            _servers.push_back(server);
            server_block.clear();
        }
    }
    close(fd);
	if (line)
		free(line);
	if (ret == -1)
		throw std::runtime_error("Error while reading config file");
	if (!server_block.empty())
		throw std::runtime_error("Error: missing '{' or '}' in config file");
    print_struct();
}

void    webserver::validate_configuration() {
    for (std::vector<server>::iterator it = _servers.begin(); it != _servers.end(); it++) {
        bool duplicate = false;
        int port = it->_port;
        int time_out = it->_time_out;
        if (port <= 0)
            throw std::invalid_argument("Error: invalid/missing port in configuration file");
        if (time_out <= 0)
            throw std::invalid_argument("Error: invalid time out in configuration file");
        for (std::vector<server>::iterator compare = _servers.begin(); compare != _servers.end(); compare++) {
            if (port == compare->_port) {
                if (duplicate == true)
                    throw std::invalid_argument("Error: duplicate port in configuration file");
                duplicate = true;
            }
        }
		std::string error_page_location = it->_error_page;
		if (error_page_location.empty())
			throw std::invalid_argument("Error: error page location missing in configuration file");
    }
}

void    webserver::establish_connection(){
	for (size_t index = 0; index < _servers.size(); index++) {
        _servers[index].create_socket();
        _servers[index].bind_socket_address(_servers[index].get_port());
        _servers[index].create_connection(1000);
	}
}

//-------------------------------------- RUN function --------------------------------------

void    webserver::run() {
	int response_count = 0;
    file_descriptors    fd;
    fd.initialize_max(_servers);
    std::cout << "--- Waiting for activity... ---" << std::endl;
    while (true)
    {
        fd.synchronize(_servers);
        if (select(fd.get_max(), &fd.get_read(), &fd.get_write(), 0, 0) == -1) {
			fd.clr_fd_sets();
			continue ;
		}
        for (size_t server_index = 0; server_index < _servers.size(); server_index++) {
            server *server = &_servers[server_index];

            if (fd.rdy_for_reading(server->get_tcp_socket())) //accept request
            {
                int newFD = accept(server->get_tcp_socket(), (struct sockaddr *) &server->_addr, (socklen_t *) &server->_addr_len);
                if (newFD == -1)
					continue ;
                if (fcntl(newFD, F_SETFL, O_NONBLOCK) == -1)
					continue ;
                fd.set_time_out(newFD);
                fd.accepted_request_update(newFD);
                server->_clients.push_back(client(newFD));

                if (server->_clients.size() == 1)
                	server->_clients.begin()->_active = true;
            }

            for (size_t client_index = 0;  client_index < server->_clients.size(); client_index++) {
                client *client_current = &server->_clients[client_index];

                try {
					set_time_out_check(server, client_current);
					if (!client_current->_active)
                		continue;

                    if (fd.rdy_for_reading(client_current->_clientFD)) //handle requested file
					{
						std::string request_headers;
						read_browser_request(request_headers, client_current->_clientFD);
						if (!request_headers.empty()) {
							_time_out_check = false;
							fd.set_time_out(client_current->_clientFD);
						}
						if (!request_headers.empty() && server->update_request_buffer(client_current->_clientFD, request_headers) == valid_) {
							client_current->_handler.parse_request(server->_request_buffer[client_current->_clientFD]);
							server->remove_handled_request(client_current->_clientFD);
							client_current->_fileFD = client_current->_handler.handle_request(server->_cgi_file_types, server->_location_blocks, server->get_error_page(), &client_current->_authorization_status);
							fd.handled_request_update(client_current->_fileFD, client_current->_clientFD, server->_cgi_file_types, client_current->_handler.verify_content_type(), client_current->_handler.get_method());
							if (server->_cgi_file_types.find(client_current->_handler.verify_content_type()) != std::string::npos) {
								client_current->_cgi_inputFD = client_current->_handler.create_cgi_fd("input");
								fd.set_write_buffer(client_current->_cgi_inputFD);
								fd.update_max(client_current->_cgi_inputFD);
							}
						}
					}

                    if (fd.rdy_for_reading(client_current->_fileFD)) //read requested file
                    {
                        if (fd.rdy_for_writing(client_current->_fileFD) && client_current->_handler.get_status() < error_ &&
							!client_current->_handler.get_write_to_file()) {
                            if (client_current->_handler.get_bytes_written() < (int)client_current->_handler.get_body().size() || (int)client_current->_handler.get_body().size() == 0) {
                                if (server->_cgi_file_types.find(client_current->_handler.verify_content_type()) != std::string::npos && fd.rdy_for_writing(client_current->_cgi_inputFD))
                                    client_current->_handler.execute_cgi(client_current->_cgi_inputFD, client_current->_fileFD, server->_server_name, server->_port, client_current->_authorization_status, client_current->_handler.get_authorization());
                                else
                                    client_current->_handler.write_body_to_file(client_current->_fileFD);
								if ((int)client_current->_handler.get_body().size() == 0)
									client_current->_handler.set_write_to_file(true);
                                continue;
                            }
                            client_current->_handler.set_bytes_written(0);
                            client_current->_handler.set_write_to_file(true);
							_time_out_check = false;
                        }
                        if (client_current->_handler.get_status() != 204 &&
							!client_current->_handler.get_read_from_file()) {
							int ret = 0;
							if (client_current->_handler.verify_content_type() == "bla" && client_current->_handler.get_method() == "POST")
                                ret = client_current->_handler.read_cgi_output_file(client_current->_fileFD, (int)client_current->_handler.get_body().size());
                            else
                                ret = client_current->_handler.read_requested_file(client_current->_fileFD);
                            if (ret)
                                continue;
                            client_current->_handler.set_bytes_read(0);
                            client_current->_handler.set_read_from_file(true);
							_time_out_check = false;
                        }
                        fd.read_request_update(client_current->_fileFD, client_current->_clientFD);
                        if (client_current->_cgi_inputFD != unused_)
                            fd.clr_from_write_buffer(client_current->_cgi_inputFD);
                    }

                    if (fd.rdy_for_writing(client_current->_clientFD)) //send response
                    {
						_time_out_check = false;

                        if (!client_current->_handler.get_bytes_written())
                            client_current->_handler.create_response(client_current->_fileFD, server->_server_name);
                        client_current->_handler.send_response(client_current->_clientFD);

						if (client_current->_handler.get_bytes_written() < (int)client_current->_handler.get_response_size())
							continue;
						client_current->_handler.set_bytes_written(0);
						fd.send_response_update(server->_clients[client_index]);
						fd.update_active_client(server->_clients, client_current->_clientFD);
						//PROGRESS MONITOR
						std::cout << GREEN << "ACTIVE CLIENTS [" << server->_clients.size() << "] CLIENT [" << client_current->_clientFD <<  "] RESPONSE NB [" << response_count << "]" << RESET << std::endl;
                        response_count++;
                    }

                    if (_time_out_check) {
						fd.update_active_client(server->_clients, client_current->_clientFD);
						fd.check_time_out(server->_clients, client_current->get_clientFD(), server->_time_out);
                    }

                } //TRY BLOCK

                catch (std::string &e) {
					clean_up_after_error(fd, server, client_current, e);
				}

            } //FOR LOOP OPENFDS

        } // FOR LOOP SERVERS

		fd.sync_maxFD(_servers);

    } //WHILE(TRUE) LOOP

}

// // // // // // // // // // // // //  HELPER functions // // // // // // // // // // // // //

void webserver::set_time_out_check(server *server, client *client_current) {
	_time_out_check = !(
			(server->_request_buffer.find(client_current->_clientFD) != server->_request_buffer.end() &&
			 server->_request_buffer[client_current->_clientFD].get_body_size() != 0)
			|| client_current->_handler.get_bytes_written());
}

void webserver::clean_up_after_error(file_descriptors &fd, server *server, const client *client_current,
									 const std::string &e) {
	if(fd.rdy_for_reading(client_current->_clientFD))
		fd.clr_from_read_buffer(client_current->_clientFD);
	if (fd.rdy_for_writing(client_current->_clientFD))
		fd.clr_from_write_buffer(client_current->_clientFD);

	if(fd.rdy_for_reading(client_current->_fileFD))
		fd.clr_from_read_buffer(client_current->_fileFD);
	if (fd.rdy_for_writing(client_current->_fileFD))
		fd.clr_from_write_buffer(client_current->_fileFD);
	if (client_current->_fileFD != unused_)
		close(client_current->_fileFD);

	if(fd.rdy_for_reading(client_current->_cgi_inputFD))
		fd.clr_from_read_buffer(client_current->_cgi_inputFD);
	if (fd.rdy_for_writing(client_current->_cgi_inputFD))
		fd.clr_from_write_buffer(client_current->_cgi_inputFD);
	if (client_current->_cgi_inputFD != unused_)
		close(client_current->_cgi_inputFD);

	if (server->_clients.size() == 1) {
		close(client_current->_clientFD);
		server->_clients.clear();
	}
	else
		server->remove_client(client_current->_clientFD);
	std::cout << RED << e << RESET << std::endl; //PRINT ERROR MESSAGE
}

// // // // // // // // // // // // //  DEBUG functions // // // // // // // // // // // // //
#include <iostream>
void webserver::print_struct() {
    for (std::vector<server>::iterator it = _servers.begin(); it != _servers.end(); it++) {
        server current = *it;

        std::cout << "------------- BEGIN SERVER BLOCK -------------\n";
        std::cout << "Port = " << current.get_port() << std::endl;
        std::cout << "Host = " << current.get_host() << std::endl;
        std::cout << "Time out = " << current.get_time_out() << std::endl;
        std::cout << "Server name = " << current.get_server_name() << std::endl;
        std::cout << "Error page = " << current.get_error_page() << std::endl;
        std::cout << "CGI file types = " << current.get_cgi_file_types() << std::endl;

        std::vector<location_context> locations = current.get_location_blocks();
        for (std::vector<location_context>::iterator it = locations.begin(); it != locations.end(); it++) {
            location_context location = *it;

            std::cout << "\n  ------------- location block -------------\n";
            std::cout << "  location = " << location.get_location_context() << std::endl;
            std::cout << "  Root = " << location.get_root() << std::endl;

            std::vector<std::string> methods = location.get_method();
            std::cout << "  Allowed methods = ";
            for (std::vector<std::string>::iterator it1 = methods.begin(); it1 != methods.end(); it1++) {
                std::string method = *it1;
                std::cout << method << " ";
            }
            std::cout << "\n";

            std::cout << "  Index = " << location.get_index() << std::endl;
            std::cout << "  Autoindex = " << location.get_autoindex() << std::endl;
			std::cout << "  Alias = " << location.get_alias() << std::endl;
			std::cout << "  Max file size = " << location.get_max_file_size() << std::endl;
			std::cout << "  Auth basic = " << location.get_auth_basic() << std::endl;
			std::cout << "  Auth user info = ";
	        std::vector<std::string> user_info = location.get_auth_user_info();

	        for (std::vector<std::string>::iterator it1 = user_info.begin(); it1 != user_info.end(); it1++) {
		        std::cout << *it1 << " ";
	        }
	        std::cout << std::endl << "  Return = ";

			std::map<int, std::string> return_map = location.get_return();
			for (std::map<int, std::string>::iterator it = return_map.begin(); it != return_map.end(); it++) {
				std::cout << "redirection_status: " << (*it).first << " " << "redirection_location: " << (*it).second << std::endl;
			}
        }
        std::cout << "\n------------- END SERVER BLOCK -------------\n\n";
    }
}
