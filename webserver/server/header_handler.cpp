/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   header_handler.cpp                                 :+:    :+:            */
/*                                                     +:+                    */
/*   By: gbouwen <marvin@codam.nl>                    +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/05/03 12:34:40 by gbouwen       #+#    #+#                 */
/*   Updated: 2021/05/28 14:11:04 by gbouwen       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include <cstdio>
#include "header_handler.hpp"

header_handler::header_handler(): _status(okay_), _status_phrases(), _write_to_file(false), _read_from_file(false), _write_to_browser(false), _bytes_written(0), _bytes_read(0), _max_file_size(0), _content_length(0), _content_type("Content-Type: text/"),
                                  _content_language("en"), _content_location(), _allow(), _method(), _file_location(),
                                  _uri_location(), _protocol(), _requested_host(), _user_agent(), _accept_charset(),
                                  _accept_language(), _authorization(), _referer(), _body(), _special_x_header(),
                                  _location_index(-1), _auth_basic("off"), _auth_type("Basic"),  _response_file(),
                                  _additional_cgi_headers() {

	_status_phrases.insert (pair(200, "OK"));
	_status_phrases.insert (pair(201, "Created"));
	_status_phrases.insert (pair(204, "No Content"));
	_status_phrases.insert (pair(301, "Moved Permanently"));
	_status_phrases.insert (pair(307, "Temporary Redirect"));
	_status_phrases.insert (pair(400, "Bad Request"));
	_status_phrases.insert (pair(401, "Unauthorized"));
	_status_phrases.insert (pair(403, "Forbidden"));
	_status_phrases.insert (pair(404, "Not Found"));
	_status_phrases.insert (pair(405, "Method Not Allowed"));
	_status_phrases.insert (pair(413, "Request Entity Too Large"));
}

header_handler::~header_handler(){
    reset_handler();
    _status_phrases.clear();
}


//-------------------------------------- PARSE functions --------------------------------------
void        header_handler::parse_request(request_buf request_buffer) {
	vector request_elements                     = str_to_vector(request_buffer.get_headers());
	parse parse_array[12]                       = { &header_handler::parse_requested_host,
													&header_handler::parse_user_agent,
													&header_handler::parse_accept_charset,
													&header_handler::parse_accept_language,
													&header_handler::parse_authorization,
													&header_handler::parse_referer,
													&header_handler::parse_content_length,
													&header_handler::parse_content_type,
													&header_handler::parse_content_language,
													&header_handler::parse_content_location,
													&header_handler::parse_special_x_header,
													&header_handler::invalid_argument};

    reset_handler(); // CAN BE REMOVED?
    vector_iterator first_line = (request_elements.begin()->empty()) ? ++request_elements.begin() : request_elements.begin();
    parse_first_line(*first_line);
    for (vector_iterator it = request_elements.begin(); it != request_elements.end(); it++) {
        int request_value = identify_request_value(*it);
        parse function = parse_array[request_value];
        (this->*function)(*it);
    }
    parse_body(request_buffer);
//    print_request(); //DEBUG
}

int         header_handler::identify_request_value(const std::string &str) {
    if (str.find("Host") != std::string::npos)
        return requested_host_;
    else if (str.find("User-Agent") != std::string::npos)
        return user_agent_;
	else if (str.find("Accept-Charset") != std::string::npos)
		return accept_charset_;
    else if (str.find("Accept-Language") != std::string::npos)
        return accept_language_;
    else if (str.find("Authorization:") != std::string::npos)
        return authorization_;
    else if (str.find("Referer:") != std::string::npos)
        return referer_;
    else if (str.find("Content-Length:") != std::string::npos)
        return content_length_;
    else if (str.find("Content-Type:") != std::string::npos)
        return content_type_;
    else if (str.find("Content-Language:") != std::string::npos)
        return content_language_;
    else if (str.find("Content-Location:") != std::string::npos)
        return content_location_;
	else if (str.find("X-") != std::string::npos)
		return special_x_header_;
    return unknown_;
}

void        header_handler::parse_first_line(const std::string &str) {
    int		index;
    size_t	start = (str[0] == '\n') ? 1 : 0;
    size_t	end = str.find_first_of(' ', start);
    _method = str.substr(start, end - start);

    start = end + 1;
    end = str.find_first_of(' ', start);
	if ((index = str.find('?', start)) != -1)
	{
		_uri_location = str.substr(start, index - start);
		index++;
		_body = str.substr(index, end - index);
	}
	else
		_uri_location = str.substr(start, end - start);
    _protocol = str.substr(end + 1);
}

void        header_handler::parse_body(request_buf request) {
    std::list<std::string> body = request.get_body();
    _body.reserve(request.get_body_size());

    if (_content_length)
        for(list_iterator it = body.begin(); it != body.end(); it++)
            _body.append(*it);
    else {
        int pos = 0;
        long int chunk_size;
        std::string str;
        str.reserve(request.get_body_size());
        for (list_iterator it = body.begin(); it != body.end(); it++)
            str.append(*it);
        while (true) {
            int len_hex_nb = (int)str.find("\r\n", pos) - pos;
            std::string hex_nb = str.substr(pos, len_hex_nb);
            if ((chunk_size = hex_to_dec(hex_nb, 16)) == 0)
                break;
            _body.append(str.substr(len_hex_nb+2, chunk_size));
            pos = (int)str.find("\r\n", pos) + chunk_size + 4;
        }
        str.clear();
    }
}

void        header_handler::parse_requested_host(const std::string &str) {_requested_host = parse_string(str);}
void        header_handler::parse_user_agent(const std::string &str) {_user_agent = parse_string(str);}
void        header_handler::parse_accept_charset(const std::string &str) {_accept_charset = parse_string(str);}
void        header_handler::parse_accept_language(const std::string &str) { _accept_language = parse_string(str);}
void        header_handler::parse_authorization(const std::string &str) {_authorization = parse_string(str);}
void        header_handler::parse_referer(const std::string &str) {_referer = parse_string(str);}
void        header_handler::parse_content_length(const std::string &str) {_content_length = parse_number(str);}
void        header_handler::parse_content_type(const std::string &str) {_content_type = parse_string(str);}
void        header_handler::parse_content_language(const std::string &str) {_content_language = parse_string(str);}
void        header_handler::parse_content_location(const std::string &str) {_content_location = parse_string(str);}
void        header_handler::parse_special_x_header(const std::string &str) {_special_x_header.push_back(str);}
void        header_handler::invalid_argument(const std::string &str) {parse_invalid(str);}


//-------------------------------------- HANDLE functions --------------------------------------

bool     validate_user_password(std::vector<std::string> correct_passwd, std::string authorization)
{
	int start = authorization.find("Basic");
	if (start == (int)std::string::npos)
		return false;
	std::string auth_passwd = authorization.substr(start + 6, std::string::npos);
	std::string decoded_auth_passwd = base64_decode(auth_passwd.c_str());
	int division = decoded_auth_passwd.find(":");
	if (division == (int)std::string::npos)
		return false;
	std::string auth_username = decoded_auth_passwd.substr(0, division);
	std::string auth_password = decoded_auth_passwd.substr(division + 1, std::string::npos);
	for (std::vector<std::string>::iterator it = correct_passwd.begin(); it != correct_passwd.end(); it++)
	{
		int div = (*it).find(":");
		if (div == (int)std::string::npos)
			return false;
		std::string username = (*it).substr(0, div);
		std::string password = (*it).substr(div + 1, std::string::npos);
		if (username == auth_username && base64_decode(password) == auth_password)
			return true;
		username.clear();
		password.clear();
	}
	return false;
}

void header_handler::verify_authorization(location_context location_block, bool *authorization_status)
{
	if (_status >= 300 && _status <= 310)
		return;
	if (location_block.get_auth_basic() != "off" && !*authorization_status)
	{
		if (_authorization.empty())	{
			_status = unauthorized_;
			_auth_basic = location_block.get_auth_basic();
		}
		else {
			if (validate_user_password(location_block.get_auth_user_info(), _authorization)) {
				_status = okay_;
				*authorization_status = true;
			}
			else {
				_status = forbidden_;
				*authorization_status = false;
			}
		}
	}
}


void	header_handler::remove_file()
{
	int	ret = remove(_file_location.c_str());
	if (ret != 0)
		_status = not_found_;
}


int header_handler::handle_request(std::string cgi_file_types, location_vector location_blocks, std::string error_page, bool *authorization_status) {
	struct  stat stats;
	int		fd = unused_;

	verify_file_location(location_blocks, error_page);
	verify_method(cgi_file_types);
	if (_location_index != -1)
		verify_authorization(location_blocks[_location_index], authorization_status);
	if (_status < moved_permanently_)
    {
		if (_method == "PUT")
			fd = put_request();
		else if (_method == "POST" && cgi_file_types.find(verify_content_type()) == std::string::npos)
			fd = post_request(_max_file_size);
		else if (_method == "DELETE")
			remove_file();
		else if (cgi_file_types.find(verify_content_type()) != std::string::npos)
		    return create_cgi_fd("output");
		else if (stat(_file_location.c_str(), &stats) == -1)
			_status = not_found_;
		else if (!(stats.st_mode & S_IRUSR))
			_status = forbidden_;
		else if ((fd = open(&_file_location[0], O_RDONLY)) == -1)
            throw (std::string("Open failed"));
    }

    if (_status >= error_code_)
    {
		char *status_str = ft_itoa(_status);
        _file_location = error_page.append(status_str);
		free(status_str);
        _file_location.append(".html");
        if ((fd = open(&_file_location[0], O_RDONLY)) == -1)
            throw (std::string("Open failed"));
    }
    if (fd != -1) {
		if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
			throw (std::string("FCNTL failed"));
	}
	return fd;
}

std::string	header_handler::get_referer_part()
{
	int			start;
	std::string	result;

	if (_referer.empty())
		return ("");
	start = _referer.find(_requested_host);
	start += _requested_host.length();
	result = _referer.substr(start, std::string::npos);
	if (result.empty())
		return ("");
	return (result);
}

std::string	header_handler::location_of_uploaded_file(location_context location_block, std::string root, std::string uri_location, std::string extension)
{
    std::string	location_from_uri;
	std::string	directory = root;
	struct stat	s;
	std::string	result = "not found";

    location_from_uri = get_first_directory(uri_location);
    if (uri_location.find("directory") == std::string::npos)
	    directory.append(get_first_directory(uri_location));
    if (stat(directory.c_str(), &s) == 0 && (s.st_mode & S_IFDIR) && location_from_uri.compare(location_block.get_location_context()) == 0) {
        result = root;
        result.append(uri_location);
    }

    if ((s.st_mode & S_IFREG) && (location_from_uri.compare(location_block.get_location_context()) == 0 && (_method == "POST" && extension == ".bla"))) {
		result = root;
	}
	return (result);
}

std::string	get_extension(std::string uri_location)
{
	int	start = uri_location.find_last_of('.');
	if (start == -1)
		return (uri_location);
	std::string	result = uri_location.substr(start, std::string::npos);

	return (result);
}

std::string	header_handler::match_location_block(header_handler::location_vector location_blocks, std::string uri_location)
{
    std::string	result;
	std::string	extension = get_extension(uri_location);
	std::string	referer_location = get_referer_part();
	referer_location = remove_duplicate_forward_slashes(referer_location);
	uri_location = remove_duplicate_forward_slashes(uri_location);

	for (size_t index = 0; index < location_blocks.size(); index++)
	{
		if (_uri_location == location_blocks[index].get_location_context())
		{
			if (!location_blocks[index].get_return().empty())
			{
				_status = location_blocks[index].get_return().begin()->first;
				_file_location = location_blocks[index].get_return().begin()->second;
				return ("http redirect");
			}
		}
		_max_file_size = location_blocks[index].get_max_file_size();
		_allow = location_blocks[index].get_method();
		_location_block_root = location_blocks[index].get_root();
		_location_index = index;
		std::string location_context = location_blocks[index].get_location_context();
		result = _location_block_root;
		if (!_referer.empty())
		{
			struct stat	s;
			result.append(referer_location);
			if (stat(result.c_str(), &s) == 0)
			{
				if (s.st_mode & S_IFREG)
				{
					int	end = result.find_last_of('/');
					result = result.substr(0, end);
				}
			}
		}
		if (location_blocks[index].get_alias() && location_context.compare(get_first_directory(uri_location)) == 0)
			result.append(skip_first_directory(uri_location));
		else if (_method.compare("PUT") == 0 || (_method.compare("POST") == 0 && (extension != ".php" || extension == ".bla"))) // add post with file upload later?
			result.append("");
		else
			result.append(uri_location);
		if (_method.compare("PUT") == 0 || (_method.compare("POST") == 0 && (extension != ".php" || extension == ".bla"))) // add post with file upload later?
			result = location_of_uploaded_file(location_blocks[index], result, uri_location, extension);
		else
			result = get_file(location_blocks[index], result);
		if (result.compare("not found") != 0)
			return (result);
	}
	_allow.clear();
	_allow.push_back("GET");
	if (_method.compare("DELETE") == 0)
		_allow.push_back("DELETE");
	return ("not found");
}

std::string	header_handler::generate_error_page_location(std::string error_page)
{
	int pos = _uri_location.find_last_of('/');
	std::string temp = _uri_location.substr(pos + 1);
	return (error_page.append(temp));
}

void        header_handler::verify_file_location(header_handler::location_vector location_blocks, std::string error_page)
{
	std::string result = match_location_block(location_blocks, _uri_location);

	if (result.compare("not found") == 0)
	{
		_file_location = generate_error_page_location(error_page);
		_location_index = -1;
	}
	else if (result.compare("http redirect") == 0)
		return;
	else
		_file_location = result;
	_file_location = remove_duplicate_forward_slashes(_file_location);
}

void header_handler::verify_method(std::string cgi_file_types)
{
	int i = 0;

	if (_status == moved_permanently_ || _status == temporary_redirect_)
		return;
	if (cgi_file_types.find(verify_content_type()) != std::string::npos)
		i++;
    if (_method == "POST" && verify_content_type() == "bla")
        return;
	else if (!_allow.empty())
	{
		for (vector_iterator it = _allow.begin(); it != _allow.end(); it++) {
			if (*it == _method)
				return;
		}
	}
	_status = method_not_allowed_;
}

std::string     header_handler::verify_content_type() {
    vector extensions;
    extensions.push_back("html");
    extensions.push_back("php");
    extensions.push_back("css");
    extensions.push_back("ico");
    extensions.push_back("png");
    extensions.push_back("bla");

    for (vector_iterator it = extensions.begin(); it != extensions.end(); it++) {
        if (_file_location.find(*it) != std::string::npos)
            return *it;
    }
    return "folder";
}

int header_handler::create_cgi_fd(std::string type)
{
	std::string str_filename = "server_files/www/temp_files/cgi_out";
	if (type == "input")
		str_filename = "server_files/www/temp_files/cgi_in";

	const char *filename = str_filename.c_str();
	int	cgiFD = open(filename, O_CREAT | O_RDWR | O_TRUNC, S_IRWXU);
	if (cgiFD == -1)
		throw (std::string("Open cgi fd failed"));
	if (fcntl(cgiFD, F_SETFL, O_NONBLOCK) == -1)
		throw (std::string("FCNTL failed"));
	return (cgiFD);
}

int     	header_handler::put_request()
{
    int			fd = unused_;
	struct stat	stats;
    std::string folder = "server_files/www/downloads/";
    std::string file = _file_location.substr(_file_location.find_last_of('/') + 1);
    std::string put_file = folder.append(file);

	if (stat(put_file.c_str(), &stats) != -1) {
        _status = no_content_;
        fd = open(&put_file[0], O_RDWR | O_TRUNC, S_IRWXU);
    }
	else {
        _status = created_;
        fd = open(&put_file[0], O_RDWR | O_CREAT, S_IRWXU);
    }
    if (fd == -1)
        throw (std::string("Open put request failed"));
    return fd;
}

int         header_handler::post_request(int max_file_size) {
    int fd = unused_;
    std::string post_file = "server_files/www/downloads/POST_file";

    if (max_file_size && max_file_size < (int)_body.length()) {
        _status = payload_too_large_;
        return fd;
    }
    _status = okay_;
    fd = open(&post_file[0], O_RDWR | O_TRUNC | O_CREAT, S_IRWXU);
    if (fd == -1)
        throw (std::string("Open post request failed"));
    return fd;
}

void        header_handler::write_body_to_file(int file_fd) {
    if (_body.empty())
        return;
    if ((_bytes_written = write(file_fd, _body.c_str(), _body.length())) == -1)
        throw (std::string("Write body to file failed"));
}




//-------------------------------------- CGI functions --------------------------------------

void header_handler::execute_cgi(int inputFD, int outputFD, std::string server_name, int server_port, bool auth_status,
                                 std::string auth_info)
{
	pid_t	pid;
	int 	status = 0;

	pid = fork();
	if (pid == -1)
		throw (std::string("fork() failed"));
	if (pid == 0) {
		try {
			char	**args = create_cgi_args();
			char 	**envp = create_cgi_envp(server_name, server_port, auth_status, auth_info);

			if (write(inputFD, _body.c_str(), _body.size()) != (int)_body.size() ||
			lseek(inputFD, 0, SEEK_SET) == -1 ||
			dup2(inputFD, STDIN_FILENO) == -1 ||
			dup2(outputFD, STDOUT_FILENO) == -1 ||
			execve(args[0], args, envp) == -1)
				exit(EXIT_FAILURE);
		}
		catch (std::string &e) {
			std::cout << e << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	else {
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_FAILURE)
			throw (std::string("execute cgi child process failure"));
		_bytes_written = _body.size();
    }
}

std::string	get_location_without_root(std::string &file_location)
{
	std::string	result;

	result = file_location.substr(file_location.find_last_of("/") + 1);
	return (result);
}

std::string create_special_x_header_envp(std::string &str)
{
		std::string special_header = "HTTP_";

		for (size_t i = 0; i < str.size(); i++)
		{
			char c = *(str.c_str() + i);
			if (c == '-')
				special_header.push_back('_');
			else if (c == ':')
				special_header.push_back('=');
			else if (c != ' ')
				special_header.push_back(toupper(c));
		}

		return special_header;
}

char	**header_handler::create_cgi_args()
{
	char	**args = new char *[3];
	char 	server_root[PATH_MAX];

	if (!getcwd(server_root, (size_t)PATH_MAX))
		throw (std::string("getcwd() failed"));
	if (_file_location.find(".php") != std::string::npos)
		args[0] = ft_strdup("/usr/bin/php");
	else if (_file_location.find(".bla") != std::string::npos)
//		args[0] = ft_strjoin(server_root, "/tester_executables/cgi_tester");
		args[0] = ft_strjoin(server_root, "/tester_executables/ubuntu_cgi_tester");

	char *tmp = ft_strjoin(server_root, "/");
	args[1] = ft_strjoin(tmp, _file_location.c_str());
	args[2] = NULL;

	return args;
}

char **header_handler::create_cgi_envp(const std::string &server_name, int server_port, bool auth_status,
                                       std::string auth_info)
{
	vector	cgi_envps;
	char 	server_root[PATH_MAX];
	if (!getcwd(server_root, (size_t)PATH_MAX))
		throw (std::string("getcwd() failed"));

	char *tmp = ft_itoa(_content_length);
	if (!tmp)
		throw (std::string("ft_itoa() failed"));
	cgi_envps.push_back(((std::string)"CONTENT_LENGTH=").append(tmp));
	free(tmp);
	cgi_envps.push_back(((std::string)"CONTENT_TYPE=").append(_content_type));
	cgi_envps.push_back((std::string)"GATEWAY_INTERFACE=CGI/1.1");
	cgi_envps.push_back((std::string)"HTTP_REFERER=");
	cgi_envps.push_back(((std::string)"PATH_INFO=").append(_uri_location));
	cgi_envps.push_back(((((std::string)"PATH_TRANSLATED=").append(server_root)).append("/")).append(_location_block_root).append(_uri_location));
	if (!_body.empty() && _uri_location.find(".bla") == std::string::npos)
		cgi_envps.push_back(((std::string)"QUERY_STRING=").append(_body));
	if (auth_status)
	{
		int start = auth_info.find("Basic");
		std::string auth_passwd = auth_info.substr(start + 6, std::string::npos);
		std::string decoded_auth_passwd = base64_decode(auth_passwd.c_str());
		int div = decoded_auth_passwd.find(":");
		std::string auth_username = decoded_auth_passwd.substr(0, div);
		cgi_envps.push_back(((std::string)"AUTH_TYPE=").append(_auth_type));
		cgi_envps.push_back(((std::string)"REMOTE_ADDR=localhost"));
		cgi_envps.push_back(((std::string)"REMOTE_IDENT="));
		cgi_envps.push_back(((std::string)"REMOTE_USER=").append(auth_username));
	}
	cgi_envps.push_back(((std::string)"REQUEST_METHOD=").append(_method));
	cgi_envps.push_back(((std::string)"REQUEST_URI=").append(_uri_location));
	cgi_envps.push_back(((((std::string)"SCRIPT_FILENAME=").append(server_root)).append("/")).append(_file_location));
	cgi_envps.push_back(((std::string)"SCRIPT_NAME=").append(get_location_without_root(_uri_location)));
	cgi_envps.push_back(((std::string)"SERVER_NAME=").append(server_name));
	char *tmp2 = ft_itoa(server_port);
	if (!tmp2)
		throw (std::string("ft_itoa() failed"));
	cgi_envps.push_back(((std::string)"SERVER_PORT=").append(tmp2));
	free(tmp2);
	cgi_envps.push_back(((std::string)"SERVER_PROTOCOL=").append(get_protocol()));
	cgi_envps.push_back((std::string)"SERVER_SOFTWARE=HTTP 1.1");
	for (vector_iterator it = _special_x_header.begin(); it != _special_x_header.end(); it++)
	{
		std::string special_header = create_special_x_header_envp(*it);
		cgi_envps.push_back(special_header);
		special_header.clear();
	}

	char 	**envp = new char *[cgi_envps.size() + 1];
	int		i = 0;

	for (vector_iterator it = cgi_envps.begin(); it != cgi_envps.end(); it++) {
		envp[i] = ft_strdup((*it).c_str());
		i++;
	}
	envp[cgi_envps.size()] = NULL;
	return envp;
}


//-------------------------------------- RESPONSE functions --------------------------------------
int        header_handler::read_requested_file(int fd) {
    int     ret;
    char    buff[6000000];

    lseek(fd, _bytes_read, SEEK_SET);
    if ((ret = read(fd, buff, 6000000)) == -1)
        throw (std::string("Read requested file failed"));
    _response_file.append(buff, ret);
    _bytes_read += ret;

    return ret;
}

int        header_handler::read_cgi_output_file(int fd, int body_size) {
    int         ret;
    char        buff[6000000];

    if (_bytes_read == 0)
        _response_file.reserve(body_size + 500);
    lseek(fd, _bytes_read, SEEK_SET);
    if ((ret = read(fd, buff, 6000000)) == -1)
        throw (std::string("Read cgi file failed"));
    _response_file.append(buff, ret);
    _bytes_read += ret;
    if (ret)
        return ret;

    if (_bytes_read) {
        int header_size = (int) _response_file.find("\r\n\r\n");
        _additional_cgi_headers = _response_file.substr(0, header_size);
        _response_file = _response_file.substr((header_size + 4), _response_file.length() - (header_size + 4));
    }
    return ret;
}

void    header_handler::create_response(int fileFD, std::string server_name) {
    response response;

    response.allocate_size(_response_file);
    response.generate_status_line(_protocol, _status, _status_phrases);
	response.generate_server_name(server_name);
	response.generate_date();
	response.generate_content_length(_response_file);
	if (_status == unauthorized_)
		response.generate_www_authorization(_auth_basic);
	if (_status != moved_permanently_ && _status != temporary_redirect_)
	{
		response.generate_content_type(verify_content_type());
		response.generate_last_modified(fileFD);
		response.generate_content_language();
	}
	if (_status == method_not_allowed_)
		response.generate_allow(_allow);
	if (_status == created_ || _status == moved_permanently_ || _status == temporary_redirect_ || ((_method == "POST" && !_body.empty() && _uri_location.find(".bla") == std::string::npos) && _uri_location.find(".php") == std::string::npos))
		response.generate_location(_status, _file_location);
	if (!_additional_cgi_headers.empty())
		response.append_cgi_headers(_additional_cgi_headers);
	response.close_header_section();

    _response.append(response.get_response());
    if (_method != "HEAD")
        _response.append(_response_file);
    _response_size = _response.size();

//    print_response_headers(response.get_response()); //DEBUG
}

void    header_handler::send_response(int clientFD) {
    int ret;

    if ((ret = write(clientFD, _response.c_str() + _bytes_written, _response.size() - _bytes_written)) == -1)
        throw (std::string("Write to browser failed"));
    else
        _bytes_written += ret;

    if (_bytes_written == (int)_response.size())
        reset_handler();
}


//-------------------------------------- RESET functions --------------------------------------
void    header_handler::reset_handler() {
    _status = 200;
    _content_length = 0;
    _max_file_size = 0;
    _bytes_written = 0;
    _response_size = 0;
    _write_to_file = false;
    _read_from_file = false;
    _write_to_browser = false;
    _method.clear();
    _file_location.clear();
    _protocol.clear();
    _response.clear();
    _response_file.clear();
    _requested_host.clear();
    _user_agent.clear();
    _accept_language.clear();
    _authorization.clear();
    _referer.clear();
    _body.clear();
    _content_type.clear();
    _content_language = "en";
    _content_location.clear();
    _additional_cgi_headers.clear();
    _allow.clear();
    _special_x_header.clear();
    _uri_location.clear();
    _location_block_root.clear();
    _accept_charset.clear();
	_location_index = -1;
	_auth_basic.clear();
}


//-------------------------------------- GET functions --------------------------------------
int                     header_handler::get_max_file_size() {return _max_file_size;}
int                     header_handler::get_status() {return _status;}
int                     header_handler::get_content_length() {return _content_length;}
int                     header_handler::get_bytes_written() {return _bytes_written;}
int                     header_handler::get_bytes_read() {return _bytes_read;}
int                     header_handler::get_response_size() {return _response_size;}
bool                    header_handler::get_write_to_file() {return _write_to_file;}
bool                    header_handler::get_read_from_file() {return _read_from_file;}
bool                    header_handler::get_write_to_browser() {return _write_to_browser;}
std::string             header_handler::get_content_type() {return _content_type;}
std::string             header_handler::get_content_language() {return _content_language;}
std::string             header_handler::get_content_location() {return _content_location;}
header_handler::vector	header_handler::get_allow() {return _allow;}
std::string	            header_handler::get_response_file() {return (_response_file);}
std::string             header_handler::get_method() {return _method;}
std::string             header_handler::get_file_location() {return _file_location;}
std::string             header_handler::get_uri_location() {return _uri_location;}
std::string				header_handler::get_location_block_root() { return _location_block_root; }
std::string             header_handler::get_protocol() {return _protocol;}
std::string             header_handler::get_requested_host() {return _requested_host;}
std::string             header_handler::get_user_agent() {return _user_agent;}
std::string             header_handler::get_accept_charset() {return _accept_charset;}
std::string             header_handler::get_accept_language() {return _accept_language;}
std::string             header_handler::get_authorization() {return _authorization;}
std::string             header_handler::get_referer() {return _referer;}
std::string             header_handler::get_body() {return _body;}
header_handler::vector  header_handler::get_special_x_header() {return _special_x_header;}
int                     header_handler::get_location_index() {return _location_index;}
std::string             header_handler::get_auth_basic() {return _auth_basic;}


//-------------------------------------- SET functions --------------------------------------
void                    header_handler::set_bytes_written(int bytes) {_bytes_written = bytes;}
void                    header_handler::set_bytes_read(int bytes) {_bytes_read = bytes;}
void                    header_handler::set_read_from_file(bool status) {_read_from_file = status;}
void                    header_handler::set_write_to_file(bool status) {_write_to_file = status;}
void                    header_handler::set_write_to_browser(bool status) {_write_to_browser = status;}


// // // // // // // // // // // // //  DEBUG functions // // // // // // // // // // // // //
void header_handler::print_request() {
    std::cout << YELLOW << "------------- REQUEST -------------\n";
    std::cout << "Request headers\n";
    std::cout << "  Method = " << get_method() << std::endl;
    std::cout << "  Location = " << get_file_location() << std::endl;
    std::cout << "  Protocol = " << get_protocol() << std::endl;
    std::cout << "  Host = " << get_requested_host() << std::endl;
    std::cout << "  User agent = " << get_user_agent() << std::endl;
    std::cout << "  Accept language = " << get_accept_language() << std::endl;
    std::cout << "  Accept charset = " << get_accept_charset() << std::endl;
    std::cout << "  Authorization = " << get_authorization() << std::endl;
    std::cout << "  Referer = " << get_referer() << std::endl;
    std::cout << "  Body = " << get_body() << std::endl;

    std::cout << "Entity headers\n";
    std::cout << "  Content length = " << get_content_length() << std::endl;
    std::cout << "  Content type = " << get_content_type() << std::endl;
    std::cout << "  Content language = " << get_content_language() << std::endl;
    std::cout << "  Content location = " << get_content_location() << std::endl;
    std::cout << "------------- END REQUEST -------------\n\n" << RESET;
}

void    header_handler::print_response_headers(std::string response) {
    std::cout << MAGENTA << "\n------------- RESPONSE HEADERS -------------\n";
    std::cout << response <<  RESET << std::endl;
}
