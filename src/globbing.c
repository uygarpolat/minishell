/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   globbing.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/26 00:45:16 by upolat            #+#    #+#             */
/*   Updated: 2024/11/08 21:47:30 by upolat           ###   ########.fr       */
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

int	append_res(char **result,
		char *d_name, int *int_array, int flag)
{
	if (*result == NULL)
		*result = ft_strdup(d_name);
	else
	{
		if (flag)
		{
			free_void((void **)result, NULL);
			*result = back_to_char(int_array);
			if (*result == NULL)
				return (-1);
			ft_putstr_fd("bash: ", 2);
			ft_putstr_fd(*result, 2);
			ft_putstr_fd(": ambiguous redirect\n", 2);
			free_void((void **)result, NULL);
			return (-1);
		}
		*result = ft_strjoin_free(*result, " ");
		if (*result == NULL)
			return (-1);
		*result = ft_strjoin_free(*result, d_name);
	}
	if (*result == NULL)
		return (-1);
	return (0);
}

int	init_expand_wildcard(DIR **dir, struct dirent **entry)
{
	*dir = opendir(".");
	if (*dir == NULL)
		return (-1);
	*entry = readdir(*dir);
	if (*entry == NULL)
	{
		closedir(*dir);
		return (-1);
	}
	return (0);
}

char	*expand_wildcard(int *int_array, t_tokens *tokens, int loc, int flag)
{
	DIR				*dir;
	struct dirent	*entry;
	char			*res;

	if (loc != 0 && identify_token(tokens[loc - 1].type))
		flag = 1;
	res = NULL;
	if (init_expand_wildcard(&dir, &entry) == -1)
		return (NULL);
	while (entry != NULL && (loc == 0 || tokens[loc - 1].type != TOKEN_HEREDOC))
	{
		if (!((*int_array & 0xFF) == '*' && entry->d_name[0] == '.')
			&& match_pattern(entry->d_name, int_array))
		{
			if (append_res(&res, entry->d_name, int_array, flag) == -1)
				return (closedir(dir), NULL);
		}
		entry = readdir(dir);
	}
	if (closedir(dir) == -1)
		return (free_void((void **)&res, NULL));
	if (res == NULL)
		return (back_to_char(int_array));
	return (res);
}
