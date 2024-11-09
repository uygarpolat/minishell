/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   globbing.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/26 00:45:16 by upolat            #+#    #+#             */
/*   Updated: 2024/11/09 21:18:57 by upolat           ###   ########.fr       */
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

int	populate_res(t_tokens **tokens, t_globber *g, int *int_array, int counter)
{
	char	*exit_message;

	exit_message = NULL;
	if (g->flag && counter > 0)
	{
		exit_message = back_to_char(int_array);
		if (exit_message == NULL)
			return (-1);
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(exit_message, 2);
		ft_putstr_fd(": ambiguous redirect\n", 2);
		free_void((void **)&exit_message, NULL);
		return (-1);
	}
	(*tokens)[g->loc].globbed[counter] = ft_strdup(g->entry->d_name);
	return (0);
}

int	init_expand_wildcard(t_globber *g, int loc)
{
	g->loc = loc;
	g->dir = opendir(".");
	if (g->dir == NULL)
		return (-1);
	g->entry = readdir(g->dir);
	if (g->entry == NULL)
	{
		closedir(g->dir);
		return (-1);
	}
	return (0);
}

int	count_matching_entries(t_tokens **tokens, int *int_array,
		t_globber *g, int counter_flag)
{
	int				counter;

	counter = 0;
	while (ft_strchrnul((char *)int_array, '*') && g->entry != NULL
		&& (g->loc == 0 || (*tokens)[g->loc - 1].type != TOKEN_HEREDOC))
	{
		if (!((*int_array & 0xFF) == '*' && g->entry->d_name[0] == '.')
			&& match_pattern(g->entry->d_name, int_array))
		{
			if (counter_flag == 1
				&& populate_res(tokens, g, int_array, counter) == -1)
				return (closedir(g->dir), -1); // Handle better?
			counter++;
		}
		g->entry = readdir(g->dir);
	}
	if (closedir(g->dir) == -1)
		return (-1); // Handle better?
	return (counter);
}

char	*expand_wildcard(int *int_array, t_tokens *tokens, int loc, int flag)
{
	int				match_count;
	t_globber		g;

	g.flag = flag;
	match_count = 0;
	if (loc != 0 && identify_token(tokens[loc - 1].type))
		g.flag = 1;
	if (init_expand_wildcard(&g, loc) == -1)
		return (NULL);
	match_count = count_matching_entries(&tokens, int_array, &g, 0);
	if (!match_count)
		return (back_to_char(int_array));
	else if (match_count < 0)
		return (NULL); // Is this fine?
	tokens[loc].globbed = ft_calloc(sizeof(char *), match_count + 1);
	if (tokens[loc].globbed == NULL)
		return (NULL); // Handle better?
	if (init_expand_wildcard(&g, loc) == -1)
		return (NULL);
	if (count_matching_entries(&tokens, int_array, &g, 1) == -1)
		return (NULL);
	return (ft_strdup(tokens[loc].globbed[0]));
}
