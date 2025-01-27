/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   populate_command_node.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 11:46:21 by upolat            #+#    #+#             */
/*   Updated: 2025/01/27 22:49:25 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/ast.h"

static int	populate_command_node_error_check(t_tokens *tokens,
				int start, int *end)
{
	int	i;
	int	k;

	i = start;
	k = *end;
	if (tokens[i].type != TOKEN_OPEN_PAREN)
	{
		while (++i <= *end)
		{
			if (tokens[i].type == TOKEN_OPEN_PAREN)
				return (syntax_error_near(tokens, i), -1);
		}
	}
	else
	{
		k = find_matching_paren(tokens, i, k);
		return (syntax_error_near(tokens, k + 1), -1);
	}
	return (0);
}

int	get_size_split(t_tokens *tokens, char *str)
{
	char	**split;
	int		i;

	split = ft_split(str, ' ');
	if (split == NULL)
		return (error_handler(NULL, NULL, tokens->code, 1), -1);
	i = 0;
	while (split[i])
		i++;
	free_2d_array((void ***)&split);
	return (i);
}

static int	populate_command_node_malloc_counter(t_tokens *tokens,
		t_ast **root, int start, int *end)
{
	int	j;
	int	k;
	int	malloc_counter;

	malloc_counter = 0;
	j = start - 1;
	while (++j <= *end)
	{
		k = 0;
		if (tokens[j].globbed)
		{
			while (tokens[j].globbed[k++])
				malloc_counter++;
		}
		else if (tokens[j].flag)
			malloc_counter += get_size_split(tokens, tokens[j].value);
		else
			malloc_counter++;
	}
	printf("malloc_counter: %d\n", malloc_counter);
	(*root)->words = ft_calloc(malloc_counter + 1, sizeof(char *));
	if ((*root)->words == NULL)
		return (error_handler(NULL, NULL, tokens->code, 1), -1);
	return (malloc_counter);
}

static int	populate_command_node_globbing(t_tokens *tokens, t_ast *root, int i)
{
	char	**temp_double_pointer;

	while (*root->words)
		root->words++;
	temp_double_pointer = tokens[i].globbed;
	while (*(tokens[i].globbed))
	{
		*root->words = ft_strdup(*(tokens[i].globbed));
		if (*root->words == NULL)
			return (error_handler(NULL, NULL, tokens->code, 1), -1);
		tokens[i].globbed++;
		root->words++;
	}
	tokens[i].globbed = temp_double_pointer;
	return (0);
}

static int	populate_command_node_empty_check(t_tokens *tokens,
				t_ast *root, int i)
{
	char	**split;
	
	while (*root->words)
		root->words++;
	if (!tokens[i].flag)
	{
		*root->words = ft_strdup(tokens[i].value);
		if (*root->words == NULL)
			return (error_handler(NULL, NULL, tokens->code, 1), -1);
	}
	else
	{
		split = ft_split(tokens[i].value, ' ');
		if (split == NULL)
			return (error_handler(NULL, NULL, tokens->code, 1), -1);
		int i = 0;
		while (split[i])
		{
			printf("%s\n", split[i]);
			*root->words = ft_strdup(split[i]);
			if (*root->words == NULL)
			{
				free_2d_array((void ***)&split);
				return (error_handler(NULL, NULL, tokens->code, 1), -1);
			}
			i++;
			root->words++;
		}
		free_2d_array((void ***)&split);
	}
	// if (*root->words == NULL)
	// {
	// 	printf("entering here\n");
	// 	return (error_handler(NULL, NULL, tokens->code, 1), -1);
	// }
	return (0);
}

int	populate_command_node(t_tokens *tokens, t_ast *root, int start, int *end)
{
	int		i;
	char	**temp;
	int		error_code;

	if (populate_command_node_error_check(tokens, start, end)
		|| populate_command_node_malloc_counter(tokens, &root, start, end) < 0)
		return (-1);
	temp = root->words;
	error_code = 0;
	i = start - 1;
	root->type = AST_COMMAND;
	while (++i <= *end && !error_code)
	{
		if (tokens[i].type == TOKEN_OPEN_PAREN
			|| tokens[i].type == TOKEN_CLOSE_PAREN)
			continue ;
		if (identify_token(tokens[i].type))
			error_code = redirection_node_creator(tokens, root, &i);
		else if (tokens[i].globbed)
			error_code = populate_command_node_globbing(tokens, root, i);
		else
			error_code = populate_command_node_empty_check(tokens, root, i);
	}
	root->words = temp;
	return (cleanup_populate_command_node(&root, &error_code));
}
