/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 14:15:53 by upolat            #+#    #+#             */
/*   Updated: 2024/05/21 12:28:45 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*clean_up_static(char **str_static);

// This function processes the buffer held in str_static
// to extract a line up to a newline character.
// It updates str_static to start immediately after the extracted
// newline, or leaves it unchanged if no newline is found.
// Returns 1 if line extraction is successful, or 0 if an error
// occurs (e.g., memory allocation failure).
int	extract_line_and_update_static(char **str_static, char **line)
{
	char	*newline_ptr;
	char	*temp;

	newline_ptr = ft_strchr_plus(*str_static, '\n');
	*line = NULL;
	if (newline_ptr)
	{
		*line = ft_str_tillchar(*str_static, '\n');
		if (!*line)
		{
			free(*line);
			return (0);
		}
		temp = ft_strdup(newline_ptr);
		if (!temp)
		{
			free(*line);
			return (0);
		}
		free(*str_static);
		*str_static = temp;
	}
	return (1);
}

// Reads data from a file descriptor into a buffer and appends it to
// str_static. It continues to read and process data until EOF or error.
// Each successful read is followed by an attempt to extract a complete
// line using extract_line_and_update_static. Returns a complete line of text
// if possible. If not, calls clean_up_static to clean up and returns NULL.
char	*read_and_process(int fd, ssize_t *bytes_read,
	char **str_static, char *buffer)
{
	char	*temp;
	char	*line;

	*bytes_read = read(fd, buffer, BUFFER_SIZE);
	while (*bytes_read > 0)
	{
		buffer[*bytes_read] = '\0';
		if (*str_static)
			temp = ft_strjoin(*str_static, buffer);
		else
			temp = ft_strdup(buffer);
		if (!temp)
			return (clean_up_static(str_static));
		free(*str_static);
		*str_static = temp;
		if (*str_static)
		{
			if (!extract_line_and_update_static(str_static, &line))
				return (clean_up_static(str_static));
			if (line != NULL)
				return (line);
		}
		*bytes_read = read(fd, buffer, BUFFER_SIZE);
	}
	return (NULL);
}

// Frees the memory allocated to str_static and sets it to NULL to prevent
// memory leaks. Returns NULL to signal an error or end of operation
// without any remaining data.
char	*clean_up_static(char **str_static)
{
	free(*str_static);
	*str_static = NULL;
	return (NULL);
}

// This function checks if any unprocessed data remains in str_static at
// EOF (no newline found). If data remains, it returns this as the final line.
// Otherwise, it cleans up by freeing str_static and returns NULL.
char	*process_remaining_data(char **str_static)
{
	char	*temp;

	if (*str_static && **str_static != '\0')
	{
		temp = ft_strdup(*str_static);
		free(*str_static);
		*str_static = NULL;
		return (temp);
	}
	free(*str_static);
	*str_static = NULL;
	return (NULL);
}

// Retrieves the next line from a file descriptor. Manages a static buffer
// to store incomplete reads between calls. Handles initial checks for valid
// input, then processes any existing data in the buffer or reads more from
// the file. Returns a single line from the file without the newline character,
// or NULL if EOF is reached or an error occurs.
char	*get_next_line(int fd)
{
	static char	*str_static[OPEN_MAX];
	char		buffer[BUFFER_SIZE + 1];
	ssize_t		bytes_read;
	char		*line;

	if (fd < 0 || BUFFER_SIZE < 1 || read(fd, NULL, 0) < 0)
		return (clean_up_static(&str_static[fd]));
	if (str_static[fd])
	{
		if (!extract_line_and_update_static(&str_static[fd], &line))
			return (clean_up_static(&str_static[fd]));
		if (line != NULL)
			return (line);
	}
	line = read_and_process(fd, &bytes_read, &str_static[fd], buffer);
	if (line != NULL)
		return (line);
	if (bytes_read == -1)
		return (clean_up_static(&str_static[fd]));
	line = process_remaining_data(&str_static[fd]);
	if (line != NULL)
		return (line);
	return (clean_up_static(&str_static[fd]));
}
