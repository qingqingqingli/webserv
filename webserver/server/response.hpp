#ifndef WEBSERVER_RESPONSE_HPP
#define WEBSERVER_RESPONSE_HPP

//General includes
#include <string>
#include <map>
#include <vector>
#include <sys/stat.h>
#include <unistd.h>
#include <exception>
#include <sys/types.h>
#include <sys/wait.h>

class response {
public:
    typedef		std::map<int, std::string>          map;
    typedef		std::vector<std::string>			vector;
    typedef		std::vector<std::string>::iterator	vector_iterator;

private:
    std::string     _response;

public:
    response();
    ~response();

    //GENERATE functions
    void            allocate_size(std::string requested_file);
    void			generate_status_line(std::string protocol, int status, response::map status_phrases);
    void			generate_content_length(std::string requested_file);
    void			generate_content_type(std::string type);
    void			generate_content_language();
    void			generate_last_modified(int file_fd);
    void			generate_date();
    void			generate_server_name(std::string server_name);
    void			generate_allow(vector allowed_methods);
    void            generate_location(int status, std::string file_location);
    void 			generate_connection_close();
    void            generate_www_authorization(std::string auth_basic);
    void            append_cgi_headers(std::string cgi_headers);
    void            close_header_section();

    //GET functions
    std::string     get_response();
};

#endif //WEBSERVER_RESPONSE_HPP
