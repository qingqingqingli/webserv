/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: roybakker <roybakker@student.codam.nl>       +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/03/16 16:23:09 by roybakker     #+#    #+#                 */
/*   Updated: 2021/05/18 14:12:45 by gbouwen       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "server/webserver.hpp"

int     main(int argc, char **argv) {
    webserver   webserver;

    if (argc != 2)
	{
		std::cout << "Start the webserver like this: $>./webserv config_file" << std::endl;
        return -1;
	}
    try {
		signal(SIGINT, signal_handler);
        webserver.load_configuration(argv[1]);
        webserver.validate_configuration();
		webserver.establish_connection();
		webserver.run();
    }
    catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }
	return (0);
}
