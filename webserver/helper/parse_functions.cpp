/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ft_atoi.c                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: gbouwen <marvin@codam.nl>                    +#+                     */
/*                                                   +#+                      */
/*   Created: 2019/10/28 13:03:13 by gbouwen       #+#    #+#                 */
/*   Updated: 2021/05/28 12:31:37 by gbouwen       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "helper.hpp"

std::vector<std::string>	parse_vector(const std::string& str) {

	std::vector<std::string>	vector;
	size_t						start = str.find_first_not_of(' ');
	size_t						pos = str.find_first_of(' ', start);
	std::string					tmp = str.substr(pos + 1);
	std::string					value;

	while ((pos = tmp.find_first_of(' ')) != std::string::npos) {
		value = tmp.substr(0, pos);
		vector.push_back(value);
		tmp = tmp.substr(pos + 1);
	}
	vector.push_back(tmp);
	return (vector);
}

std::string	parse_string (const std::string& str) {

	size_t start = str.find_first_not_of(' ');
	size_t pos = str.find_first_of(' ', start);
	return (str.substr(pos + 1));
}

int parse_number (const std::string& str) {

	size_t start = str.find_first_not_of(' ');
	size_t pos = str.find_first_of(' ', start);
	std::string temp = str.substr(pos + 1);
	return (ft_atoi(temp.c_str()));
}

void parse_invalid(const std::string& str) {
	if (str == "0")
		return;
}

vector    str_to_vector(std::string request) {
    std::vector<std::string> request_elements;
    size_t      pos = 0;
    std::string value;

    while ((int)(pos = request.find_first_of('\r')) != -1) {
        value = request.substr(0, pos);
        request_elements.push_back(value);
        request = request.substr(pos + 2);
    }
    request_elements.push_back(request);
    return request_elements;
}

void read_browser_request(std::string &request, int fd) {
    char        buff[6000000];
    int         ret = 1;

    request.reserve(1);
    ret = read(fd, buff, 6000000);
	if (ret == -1)
		throw (std::string("Read browser request failed"));
    buff[ret] = '\0';
    request.append(buff, ret);
}
