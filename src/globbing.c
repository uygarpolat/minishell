/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   globbing.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/26 00:45:16 by upolat            #+#    #+#             */
/*   Updated: 2024/10/27 22:34:38 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#include "../../library/libft/libft.h"
#include "../includes/tokenizer.h"
#include "../includes/ast.h"

char	*back_to_char(int *int_array)
{
	int		i;
	char	*str;

	i = 0;
	while (int_array[i])
		i++;
	str = malloc(sizeof(char) * (i + 1));
	if (str == NULL)
		return (NULL);
	str[i] = '\0';
	while (--i >= 0)
		str[i] = (char)int_array[i];
	return (str);
}

int	match_pattern(char *filename, int *pattern)
{
	if (*pattern == '\0' && *filename == '\0')
		return (1);
	if (((*pattern & 0xFF) == '*') && ((*pattern >> 8) & 1))
	{
		while (*filename)
		{
			if (match_pattern(filename, pattern + 1))
				return (1);
			filename++;
		}
		return (match_pattern(filename, pattern + 1));
	}
	if (((*pattern & 0xFF) == '*') && !((*pattern >> 8) & 1))
	{
		if (*pattern == *filename)
			return (match_pattern(filename + 1, pattern + 1));
		else
			return (0);
	}
	if (*pattern == *filename)
		return (match_pattern(filename + 1, pattern + 1));
	return (0);
}

char	*expand_wildcard(int *int_array)
{
	DIR				*dir;
	struct dirent	*entry;
	char			*result;

	result = NULL;
	dir = opendir(".");
	if (dir == NULL)
		return (NULL); // Handle better!
	entry = readdir(dir);
	while (entry != NULL)
	{
		if (match_pattern(entry->d_name, int_array))
		{
			if (result == NULL)
				result = ft_strdup(entry->d_name);
			else
			{
				result = ft_strjoin_free(result, " ");
				result = ft_strjoin_free(result, entry->d_name);
			}
		}
		entry = readdir(dir);
	}
	closedir(dir);
	if (result == NULL)
		return (back_to_char(int_array));
	else
		return (result);
}
