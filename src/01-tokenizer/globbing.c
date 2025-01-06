/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   globbing.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/26 00:45:16 by upolat            #+#    #+#             */
/*   Updated: 2025/01/06 13:51:46 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/tokenizer.h"

int	identify_token(t_token_type type);

static int	match_pattern(char *filename, int *pattern)
{
	if (*pattern == '\0' && *filename == '\0')
		return (1);
	if (((*pattern & 0xFF) == '*') && ((*pattern >> 9) & 1))
	{
		while (*filename)
		{
			if (match_pattern(filename, pattern + 1))
				return (1);
			filename++;
		}
		return (match_pattern(filename, pattern + 1));
	}
	if (((*pattern & 0xFF) == '*') && !((*pattern >> 9) & 1))
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

static int	populate_res(t_tokens **tokens, t_globber *g,
				int *int_array, int counter)
{
	char	*exit_message;

	exit_message = NULL;
	if (g->flag && counter > 0)
	{
		exit_message = back_to_char(*tokens, int_array);
		if (exit_message == NULL)
			return (-1);
		error_handler(exit_message, "ambiguous redirect", (*tokens)->code, 1);
		free_void((void **)&exit_message, NULL);
		return (-1);
	}
	(*tokens)[g->loc].globbed[counter] = ft_strdup(g->entry->d_name);
	if ((*tokens)[g->loc].globbed[counter] == NULL)
		return (error_handler(NULL, NULL, (*tokens)->code, 1), -1);
	return (0);
}

static int	init_expand_wildcard(t_tokens *tokens, t_globber *g, int loc)
{
	g->loc = loc;
	g->dir = opendir(".");
	if (g->dir == NULL)
		return (error_handler(NULL, NULL, tokens->code, 1), -1);
	g->entry = readdir(g->dir);
	if (g->entry == NULL)
	{
		closedir(g->dir);
		return (error_handler(NULL, NULL, tokens->code, 1), -1);
	}
	return (0);
}

static int	count_matching_entries(t_tokens **tokens, int *int_array,
		t_globber *g, int counter_flag)
{
	int	counter;

	counter = 0;
	while (ft_strchrnul((char *)int_array, '*') && g->entry != NULL
		&& (g->loc == 0 || (*tokens)[g->loc - 1].type != TOKEN_HEREDOC))
	{
		if (!((*int_array & 0xFF) == '*' && g->entry->d_name[0] == '.')
			&& match_pattern(g->entry->d_name, int_array))
		{
			if (counter_flag == 1
				&& populate_res(tokens, g, int_array, counter) == -1)
				return (closedir(g->dir), -1);
			counter++;
		}
		g->entry = readdir(g->dir);
	}
	if (closedir(g->dir) == -1)
		return (error_handler(NULL, NULL, (*tokens)->code, 1), -1);
	return (counter);
}

char	*expand_wildcard(int *int_array, t_tokens *tokens, int loc, int flag)
{
	int			match_count;
	t_globber	g;
	char		*globbed;

	g.flag = flag;
	match_count = 0;
	if (loc != 0 && identify_token(tokens[loc - 1].type))
		g.flag = 1;
	if (!ft_strchr((char *)int_array, '*'))
		return (back_to_char(tokens, int_array));
	if (init_expand_wildcard(tokens, &g, loc) == -1)
		return (NULL);
	match_count = count_matching_entries(&tokens, int_array, &g, 0);
	if (!match_count)
		return (back_to_char(tokens, int_array));
	else if (match_count < 0)
		return (NULL);
	tokens[loc].globbed = ft_calloc(sizeof(char *), match_count + 1);
	if (tokens[loc].globbed == NULL)
		return (error_handler(NULL, NULL, tokens->code, 1), NULL);
	if (init_expand_wildcard(tokens, &g, loc) == -1
		|| count_matching_entries(&tokens, int_array, &g, 1) == -1)
		return (NULL);
	globbed = ft_strdup(tokens[loc].globbed[0]);
	if (globbed == NULL)
		return (error_handler(NULL, NULL, tokens->code, 1), NULL);
	return (globbed);
}
