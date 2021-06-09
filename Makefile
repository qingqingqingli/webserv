# **************************************************************************** #
#                                                                              #
#                                                         ::::::::             #
#    Makefile                                           :+:    :+:             #
#                                                      +:+                     #
#    By: roybakker <roybakker@student.codam.nl>       +#+                      #
#                                                    +#+                       #
#    Created: 2021/02/25 14:55:06 by roybakker     #+#    #+#                  #
#    Updated: 2021/05/18 14:53:56 by gbouwen       ########   odam.nl          #
#                                                                              #
# **************************************************************************** #

NAME			=	webserv

SOURCES			=	webserver/main.cpp \
                    webserver/gnl/get_next_line.cpp \
                    webserver/gnl/get_next_line_utils.cpp \
                    webserver/server/location_context.cpp \
                    webserver/server/file_descriptors.cpp \
                    webserver/server/response.cpp \
                    webserver/server/server.cpp \
                    webserver/server/webserver.cpp \
                    webserver/server/header_handler.cpp \
                    webserver/server/request_buf.cpp \
                    webserver/server/client.cpp \
                    webserver/helper/check_functions.cpp \
					webserver/helper/lib_functions.cpp \
					webserver/helper/parse_functions.cpp \
					webserver/helper/signal_handler.cpp \
					webserver/helper/verify_file_location_functions.cpp \
					webserver/server/Base64.cpp


TEMP_FILES      =   server_files/www/temp_files/cgi_in \
                    server_files/www/temp_files/cgi_out \
                    server_files/www/downloads/file_should_exist_after \
                    server_files/www/downloads/POST_file \
                    server_files/www/downloads/multiple_same

OBJECTS 		=	${SOURCES:%.cpp=%.o}

FLAGS 			=	-Wall -Wextra -Werror -Ofast -std=c++98

COMPILE			=	clang++

GREEN 			= 	\033[38;5;46m
WHITE 			= 	\033[38;5;15m
GREY 			= 	\033[38;5;8m
ORANGE 			= 	\033[38;5;202m
RED 			= 	\033[38;5;160m
RESET			= 	\033[0m

all: $(NAME)

$(NAME): $(OBJECTS)
	@echo "$(GREEN)----------------------------------------------------"
	@$(COMPILE) $(FLAGS) -o $(NAME) $(OBJECTS)
	@echo "Executable				./webserv"
	@echo "----------------------------------------------------$(RESET)"

%.o: %.cpp
	@echo "$(GREY)Compiling...	$(GREY)$<"
	@$(COMPILE) $(FLAGS) -c -o $@ $<

clean:
	@echo "$(RED)----------------------------------------------------"
	@echo "$(GREY)DELETE ./webserv"
	@echo "$(RED)----------------------------------------------------"
	@/bin/rm -f $(NAME) $(TEMP_FILES) $(OBJECTS)

re: clean all

.PHONY: all clean re
