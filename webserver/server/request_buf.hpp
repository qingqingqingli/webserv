#ifndef WEBSERVER_REQUEST_BUF_HPP
#define WEBSERVER_REQUEST_BUF_HPP

#include <string>
#include <list>

class request_buf {
public:
    typedef std::list<std::string>::reverse_iterator rev_it;

private:
    std::string             _headers;
    std::list<std::string>  _body;
    size_t                  _body_size;

public:
    request_buf();
    ~request_buf();

    //ADD_DATA functions
    void    add_header_data(const std::string& request_data);
    void    add_body_data(const std::string& request_data);

    //CHECK functions
    int     headers_received();
    int     validate_request();

    //GET functions
    std::string                 get_headers();
    std::list<std::string>      get_body();
    int                         get_body_size();

	//SET functions

	void                        set_body_size(int size);
};


#endif //WEBSERVER_REQUEST_BUF_HPP
