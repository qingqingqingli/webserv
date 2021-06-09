[![Logo](https://github.com/qingqingqingli/readme_images/blob/master/codam_logo_1.png)](https://github.com/qingqingqingli/webserv)

# webserv
***This project is to create a HTTP server in C++. Concepts about socket programming are introduced.***

HTTP is the foundation of data communication for the World Wide Web. With this project, we build a webserver from scratch, which can be tested with a real browser. 

This is a group project completed by [Q. Li](https://github.com/qingqingqingli) & [R. Bakker](https://github.com/rbakker96) & [G. Bouwen](https://github.com/gbouwen).

## Technical considerations
> Full details are included in the `webserv_subject.pdf` file

- Use only one `select` for all the IO between the client and server
- `select` should check read and write at the same time. It should never block, and the client should bounce properly if necessary
- Never do a read or write operation without going through `select`
- A request to your server should never hang forever
- You server should have default `error pages`
- You can’t use fork for something else than `CGI`

## Handled HTTP headers  
- General header: `Date`
- Request header: `User-Agent`, `Referer`, `Host`, `Authorization`
- Response header: `Server`, `Last-Modified`, `Transfer-Encoding`, `WWW-Authenticate`, `Location`
- Entity header: `Content-Length`, `Content-Type`, `Content-Language`, `Content-Location`, `Allow`

## How to test
> Disclaimer: The current version works on Ubuntu distributions. 

```shell
$ git clone https://github.com/qingqingqingli/webserv
$ cd webserv
$ make
# run the executable with one valid config provided
$ ./webserv configuration_files/valid/default.conf
```

## Demo
> The demo below shows how to run the webserv with the default config file. It demonstrated the loading of a static webpage, cgi output, authentication, autoindex, HTTP redirection. 

[![demo](https://github.com/qingqingqingli/webserv/blob/main/images/webserv.gif)](https://github.com/qingqingqingli/webserv)
