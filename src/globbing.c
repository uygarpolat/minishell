/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   globbing.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/26 00:45:16 by upolat            #+#    #+#             */
/*   Updated: 2024/11/09 19:56:04 by upolat           ###   ########.fr       */
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
/*
int	populate_res(t_tokens **tokens,
		char *d_name, int *int_array, int flag, int counter)
{
	char *exit_message;

	exit_message = NULL;

	if (!flag)
		(*tokens)->globbed[counter] = ft_strdup(d_name);
	else if (counter > 0)
	{
		exit_message = back_to_char(int_array);
		if (exit_message == NULL)
				return (-1);
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(exit_message, 2);
		ft_putstr_fd(": ambiguous redirect\n", 2);
		free_void((void **)&exit_message, NULL);
		//free_2d_array((void ***)&(*tokens)->globbed); // Do it here or nah?
	}
	return (0);
}
*/
int	populate_res(t_tokens **tokens, int loc,
		char *d_name, int *int_array, int flag, int counter)
{
	char *exit_message;

	exit_message = NULL;

	//if (!flag)
	//	(*tokens)[loc].globbed[counter] = ft_strdup(d_name);
	if (flag && counter > 0)
	{
		exit_message = back_to_char(int_array);
		if (exit_message == NULL)
				return (-1);
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(exit_message, 2);
		ft_putstr_fd(": ambiguous redirect\n", 2);
		free_void((void **)&exit_message, NULL);
		//free_2d_array((void ***)&(*tokens)->globbed); // Do it here or nah? Also the variable name is written wrong here, correct it.
		return (-1);
	}
	(*tokens)[loc].globbed[counter] = ft_strdup(d_name);
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

int	count_matching_entries(t_tokens **tokens, int *int_array, DIR *dir, struct dirent *entry, int loc, int flag, int counter_flag)
{
	int				counter;

	counter = 0;
	while (ft_strchrnul((char *)int_array, '*') && entry != NULL && (loc == 0 || (*tokens)[loc - 1].type != TOKEN_HEREDOC))
	{
		if (!((*int_array & 0xFF) == '*' && entry->d_name[0] == '.')
			&& match_pattern(entry->d_name, int_array))
		{
			if (counter_flag == 1 && populate_res(tokens, loc, entry->d_name, int_array, flag, counter) == -1)
				return (closedir(dir), -1); // Handle better?
			counter++;
		}
		entry = readdir(dir);
	}
	if (closedir(dir) == -1)
		return (-1); // Handle better?
	return (counter);
}

char	*expand_wildcard(int *int_array, t_tokens *tokens, int loc, int flag)
{
	DIR				*dir;
	struct dirent	*entry;
	int				match_count;

	match_count = 0;
	if (loc != 0 && identify_token(tokens[loc - 1].type))
		flag = 1;
	if (init_expand_wildcard(&dir, &entry) == -1)
		return (NULL);
	match_count = count_matching_entries(&tokens, int_array, dir, entry, loc, flag, 0);
	if (!match_count)
		return (back_to_char(int_array));
	else if (match_count < 0)
		return (NULL); // Is this fine?
	tokens[loc].globbed = ft_calloc(sizeof(char *), match_count + 1);
	if (tokens[loc].globbed == NULL)
		return (NULL); // Handle better?
	if (init_expand_wildcard(&dir, &entry) == -1)
		return (NULL);
	if (count_matching_entries(&tokens, int_array, dir, entry, loc, flag, 1) == -1)
		return (NULL);
	return (ft_strdup(tokens[loc].globbed[0]));
}
