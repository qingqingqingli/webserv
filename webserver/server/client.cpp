#include "client.hpp"

client::client() : _active(false), _clientFD(-1), _fileFD(-1), _cgi_inputFD(-1), _authorization_status(false), _handler() {}

client::client(int newFD) : _active(false), _clientFD(newFD), _fileFD(-1), _cgi_inputFD(-1), _authorization_status(false), _handler() {}

client::~client() {}

//-------------------------------------- GET functions --------------------------------------
int         client::get_clientFD() {return _clientFD;}
int         client::get_fileFD() {return _fileFD;}
int         client::get_cgi_inputFD() {return _cgi_inputFD;}
bool        client::get_authorization_status() {return _authorization_status;}
bool 		client::get_active() {return _active;}


//-------------------------------------- SET functions --------------------------------------
void        client::set_clientFD(int fd) {_clientFD = fd;}
void        client::set_fileFD(int fd) {_fileFD = fd;}
void        client::set_cgi_inputFD(int fd) {_cgi_inputFD = fd;}
void        client::set_authorization_status(bool status) {_authorization_status = status;}
void 		client::set_active(bool status) {_active = status;}
