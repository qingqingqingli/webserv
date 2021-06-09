/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   location_context.cpp                               :+:    :+:            */
/*                                                     +:+                    */
/*   By: roybakker <roybakker@student.codam.nl>       +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/03/30 14:07:20 by roybakker     #+#    #+#                 */
/*   Updated: 2021/05/17 15:40:28 by gbouwen       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "location_context.hpp"
#include "../helper/helper.hpp"

location_context::location_context() : _location_context(), _root(), _index(), _allowed_method(0), _max_file_size(0), _autoindex(false), _alias(false), _auth_basic(), _auth_user_info(), _return() {}
location_context::~location_context(){}


//-------------------------------------- CONFIG functions --------------------------------------
void location_context::configure_location_block(vector_iterator it, vector_iterator end) {
    configure configure_array[10] = { &location_context::configure_root,
                                     &location_context::configure_allowed_method,
                                     &location_context::configure_autoindex,
                                     &location_context::configure_index,
									 &location_context::configure_alias,
                                     &location_context::configure_max_file_size,
                                     &location_context::configure_auth_basic,
                                     &location_context::configure_auth_user_info,
                                     &location_context::configure_return,
                                     &location_context::invalid_element };

    clean_location_instance(); // CAN BE REMOVED?
	configure_location_context(*it);
    for(; it != end; it++) {
        int location_value = identify_location_value(*it);
        configure function = configure_array[location_value];
        (this->*function)(*it);
    }
}

void    location_context::clean_location_instance() {
	_location_context.clear();
	_root.clear();
	_index.clear();
	_allowed_method.clear();
	_max_file_size = 0;
	_autoindex = false;
	_alias = false;
	_auth_basic.clear();
	_auth_user_info.clear();

}

int     location_context::identify_location_value(const std::string &str){
    if (str.find("root") != std::string::npos)
        return root_;
    else if (str.find("allowed_method") != std::string::npos)
        return method_;
    else if (str.find("autoindex") != std::string::npos)
        return autoindex_;
    else if (str.find("index") != std::string::npos)
        return index_;
	else if (str.find("alias") != std::string::npos)
		return alias;
    else if (str.find("max_file_size") != std::string::npos)
        return max_file_size_;
    else if (str.find("auth_basic ") != std::string::npos)
	    return auth_basic_;
    else if (str.find("auth_user_file ") != std::string::npos)
	    return auth_user_info_;
	else if (str.find("return ") != std::string::npos)
		return return_;
	return unknown_;
}

void    location_context::configure_location_context(const std::string &str) {
    size_t start = str.find_first_of('/');
    size_t end = str.find_first_of(' ', start);

	_location_context = str.substr(start, end - start);
}

void    location_context::configure_root(const std::string &str){_root = parse_string(str);}
void    location_context::configure_allowed_method(const std::string &str){_allowed_method = parse_vector(str);}
void    location_context::configure_index(const std::string &str){_index = parse_string(str);}
void    location_context::invalid_element(const std::string &str) {parse_invalid(str);}
void    location_context::configure_max_file_size(const std::string &str) {_max_file_size = parse_number(str);}
void    location_context::configure_auth_basic(const std::string &str){_auth_basic = parse_string(str);}

void    location_context::configure_auth_user_info(const std::string &str)
{
	std::string auth_file_path = parse_string(str);
	int fd = open(auth_file_path.c_str(), O_RDONLY);
	int ret = 0;
	char *line = NULL;

	if (fd == -1)
		throw std::invalid_argument("Error opening user authorization file");
	while ((ret = get_next_line(fd, &line)) == 1) {
		_auth_user_info.push_back(line);
		free(line);
	}
	close(fd);
	if (line)
		free(line);
	if (ret == -1)
		throw std::runtime_error("Error while reading user authorization file");
}

void    location_context::configure_autoindex(const std::string &str){
	if (parse_string(str) == "on")
        _autoindex = true;
}


void    location_context::configure_alias(const std::string &str){
	if (parse_string(str) == "on")
		_alias = true;
}

void	location_context::configure_return(const std::string &str) {
	int redirect_code = parse_number(str);
	if (redirect_code < 300 || redirect_code > 310)
		throw std::runtime_error("Wrong redirection code");

	size_t status_code_start = str.find_first_of('3');
	size_t pos = str.find_first_of(' ', status_code_start);
	std::string new_location = str.substr(pos + 1);

	_return.insert(std::pair<int, std::string>(redirect_code, new_location));
}

//-------------------------------------- GET functions --------------------------------------
std::string                 location_context::get_location_context() {return _location_context;}
std::string                 location_context::get_root() {return _root;}
std::string                 location_context::get_index() {return _index;}
std::vector<std::string>    location_context::get_method() {return _allowed_method;}
int                         location_context::get_max_file_size() {return _max_file_size;}
bool                        location_context::get_autoindex() {return _autoindex;}
bool						location_context::get_alias() {return _alias;}
std::string                 location_context::get_auth_basic() {return _auth_basic;}
std::vector<std::string>    location_context::get_auth_user_info() {return _auth_user_info;}
std::map<int, std::string>  location_context::get_return() {return _return;}
