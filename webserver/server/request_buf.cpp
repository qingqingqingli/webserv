#include "request_buf.hpp"
#include "../helper/helper.hpp"

request_buf::request_buf() :_headers(), _body(), _body_size(0) {}
request_buf::~request_buf() {
    _headers.clear();
    _body.clear();
}

//-------------------------------------- ADD_DATA functions --------------------------------------
void    request_buf::add_header_data(const std::string &request_data) {
    size_t pos;

    _headers.append(request_data);
    if ((pos = _headers.find("\r\n\r\n")) != std::string::npos) {
        if ((pos+4) == _headers.size())
            return;
        add_body_data(_headers.substr(pos+4));
        _headers = _headers.substr(0, pos+4);
    }
}

void    request_buf::add_body_data(const std::string &request_data) {
    _body.push_back(request_data);
    _body_size += request_data.length();
}


//-------------------------------------- CHECK functions --------------------------------------
int     request_buf::headers_received() {
    if (_headers.find("\r\n\r\n") != std::string::npos)
        return 1;
    return 0;
}

int     request_buf::validate_request() {
    int end_of_line = 0;
    int zero = 0;
    int pos;

    if ((pos = (int) _headers.find("Content-Length:")) != -1) {
        size_t content_length = ft_atoi(_headers.c_str() + (pos + 16));
        if (content_length == _body_size)
            return valid_;
    }
    else if (_headers.find("chunked") != std::string::npos) {
        for(rev_it it = _body.rbegin(); it != _body.rend(); it++) {
            std::string str = *it;
            for (int i = (int)str.length() - 1; i >= 0; i--) {
                if (str[i] == '\r' || str[i] == '\n')
                    end_of_line++;
                else if (str[i] == '0')
                    zero++;
                else
                    return invalid_;
                if (zero == 1 && end_of_line == 4)
                    return valid_;
            }
        }
    }
    else if (_headers.find("Content-Length:") == std::string::npos && _headers.find("chunked") == std::string::npos)
        return valid_;
    return invalid_;
}


//-------------------------------------- GET functions --------------------------------------
std::string                 request_buf::get_headers() {return _headers;}
std::list<std::string>      request_buf::get_body() {return _body;}
int                         request_buf::get_body_size() {return _body_size;}

//-------------------------------------- SET functions --------------------------------------

void                         request_buf::set_body_size(int size) {_body_size = size;}
