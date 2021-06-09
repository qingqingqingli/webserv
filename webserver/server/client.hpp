#ifndef WEBSERVER_CLIENT_HPP
#define WEBSERVER_CLIENT_HPP

//custom includes
#include "header_handler.hpp"

class client {
public:
    friend class webserver;
    friend class file_descriptors;

private:
	//Status
	bool 				_active;

    //File descriptors
    int                 _clientFD;
    int                 _fileFD;
    int 	            _cgi_inputFD;
    bool                _authorization_status;

    //Handler
    header_handler      _handler;

public:
    client();
    client(int newFD);
    ~client();

    //GET functions
    int         get_clientFD();
    int         get_fileFD();
    int         get_cgi_inputFD();
    bool        get_authorization_status();
    bool		get_active();

    //SET functions
    void        set_clientFD(int fd);
    void        set_fileFD(int fd);
    void        set_cgi_inputFD(int fd);
    void        set_authorization_status(bool status);
    void		set_active(bool status);
};


#endif //WEBSERVER_CLIENT_HPP
