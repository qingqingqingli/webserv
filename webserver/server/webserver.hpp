/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   webserver.hpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: roybakker <roybakker@student.codam.nl>       +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/03/30 16:28:34 by roybakker     #+#    #+#                 */
/*   Updated: 2021/05/28 11:09:33 by gbouwen       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_WEBSERVER_HPP
#define WEBSERV_WEBSERVER_HPP

//general includes
#include <fcntl.h>
#include <cerrno>

//custom includes
#include "client.hpp"
#include "server.hpp"
#include "file_descriptors.hpp"

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

class webserver {
public:
	typedef     			std::vector<std::string>      			vector;
	typedef     			std::vector<std::string>::iterator      vector_iterator;
    typedef     			std::vector<client>::iterator           client_iterator;
    enum        			webserver_values{ unused_ = -1, ready_for_use_ = -1, error_ = 400};

private:
    std::vector<server>     _servers;
    bool                    _time_out_check;

public:
    webserver();
    ~webserver();

    //GENERAL functions
    void    				load_configuration(char* config_file);
    void        			validate_configuration();
    void    				establish_connection();
    void    				run();

	//HELPER functions
	void 					set_time_out_check(server *server, client *client_current);
	static void 			clean_up_after_error(file_descriptors &fd, server *server, const client *client_current,
									 const std::string &e) ;
    //DEBUG functions
    void        			print_struct();
};

#endif //WEBSERV_WEBSERVER_HPP
