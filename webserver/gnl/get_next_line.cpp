/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   get_next_line.c                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: rbakker <rbakker@student.codam.nl>           +#+                     */
/*                                                   +#+                      */
/*   Created: 2019/11/11 10:55:43 by rbakker        #+#    #+#                */
/*   Updated: 2019/12/11 17:09:07 by rbakker       ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.hpp"

static int		generate_output(int res, char **buf)
{
	if (res == 0)
	{
		free(*buf);
		*buf = 0;
		return (0);
	}
	return (1);
}

static int		save_remainder(char **buf, int i, int x, int size)
{
	char	*remainder;

	while ((*buf)[i] != '\n' && (*buf)[i] != '\0')
		i++;
	while ((*buf)[i + size] != '\0')
		size++;
	remainder = (char*)malloc(size + 1);
	if (!remainder)
		return (cleanup(buf));
	while ((*buf)[i + 1] != '\0' && size != 0)
	{
		remainder[x] = (*buf)[i + 1];
		x++;
		i++;
	}
	remainder[x] = '\0';
	free(*buf);
	*buf = remainder;
	return (1);
}

static int		save_line(char **buf, char **line, int size, int i)
{
	char	*new_line;

	while ((*buf)[size] != '\n' && (*buf)[size] != '\0')
		size++;
	new_line = (char*)malloc(size + 1);
	if (!new_line)
		return (cleanup(buf));
	while ((*buf)[i] != '\n' && (*buf)[i] != '\0')
	{
		new_line[i] = (*buf)[i];
		i++;
	}
	new_line[i] = '\0';
	*line = new_line;
	return (1);
}

int		get_next_line(int fd, char **line)
{
	static char	*buf;
	char		b[BUFFER_SIZE + 1];
	int			res;
	int			fail;

	if (fd < 0 || !line || read(fd, 0, 0) == -1)
		return (-1);
	while (!line_break(buf))
	{
		res = read(fd, b, BUFFER_SIZE);
		b[res] = '\0';
		fail = (buf == 0) ? cpy(&buf, b) : join_free(&buf, b, 0, 0);
		if (fail == -1)
			return (-1);
		if (res == 0)
			break ;
	}
	if (save_line(&buf, line, 0, 0) == -1)
		return (-1);
	res = return_value(buf);
	if (save_remainder(&buf, 0, 0, 0) == -1)
		return (-1);
	return (generate_output(res, &buf));
}
