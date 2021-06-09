/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   location_context.hpp                               :+:    :+:            */
/*                                                     +:+                    */
/*   By: roybakker <roybakker@student.codam.nl>       +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/03/30 14:07:28 by roybakker     #+#    #+#                 */
/*   Updated: 2021/05/17 15:42:41 by gbouwen       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_LOCATION_CONTEXT_H
#define WEBSERV_LOCATION_CONTEXT_H

//general includes
#include <string>
#include <vector>
#include <map>
#include <fcntl.h>
#include "../gnl/get_next_line.hpp"

class location_context {
public:
    typedef     std::vector<std::string>::iterator vector_iterator;
    typedef     void (location_context::*configure)(const std::string&);
    enum        location_values{ root_ = 0, method_ = 1, autoindex_ = 2, index_ = 3, alias = 4, max_file_size_ = 5, auth_basic_ = 6, auth_user_info_ = 7, return_ = 8, unknown_ = 9 };

private:
    std::string                 _location_context;
    std::string                 _root;
    std::string                 _index;
    std::vector<std::string>    _allowed_method;
    int                         _max_file_size;
    bool                        _autoindex;
	bool						_alias;
	std::string                 _auth_basic;
	std::vector<std::string>    _auth_user_info;
	std::map<int, std::string>	_return;

public:
    location_context();
    ~location_context();

    //CONFIG functions
    void    configure_location_block(vector_iterator it, vector_iterator end);
    void    clean_location_instance();
    int     identify_location_value(const std::string &str);
    void    configure_location_context(const std::string &str);
    void    configure_root(const std::string &str);
    void    configure_allowed_method(const std::string &str);
    void    configure_index(const std::string &str);
    void    configure_autoindex(const std::string &str);
	void	configure_alias(const std::string &str);
	void    configure_max_file_size(const std::string &str);
	void    configure_auth_basic(const std::string &str);
	void    configure_auth_user_info(const std::string &str);
	void    configure_return(const std::string &str);
    void    invalid_element(const std::string &str);

    //GET functions
    std::string                 get_location_context();
    std::string                 get_root();
    std::string                 get_index();
    std::vector<std::string>    get_method();
    int                         get_max_file_size();
    bool                        get_autoindex();
	bool						get_alias();
	std::string                 get_auth_basic();
	std::vector<std::string>    get_auth_user_info();
	std::map<int, std::string>	get_return();


};

#endif //WEBSERV_LOCATION_CONTEXT_H
