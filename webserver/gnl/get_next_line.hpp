/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   get_next_line.h                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: rbakker <rbakker@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2019/11/11 10:55:28 by rbakker        #+#    #+#                */
/*   Updated: 2019/12/03 17:21:33 by rbakker       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H
# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 1
# endif
# include <unistd.h>
# include <stdlib.h>

int			get_next_line(int fd, char **line);

int			cpy(char **buf, char *s1);
int			join_free(char **s1, char *s2, int i, int x);
int			line_break(char *s);
int		    return_value(char *s);
int			cleanup(char **string1);

#endif
