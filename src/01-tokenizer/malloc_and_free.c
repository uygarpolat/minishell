/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_and_free.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 00:25:49 by upolat            #+#    #+#             */
/*   Updated: 2025/01/22 21:35:11 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/tokenizer.h"

void	free_tokens(t_tokens **tokens, t_capacity *capacity)
{
	int	i;

	if (tokens == NULL || *tokens == NULL)
		return ;
	i = 0;
	while (i < capacity->max_size)
	{
		free_void((void **)&(*tokens)[i].value, NULL);
		free_2d_array((void ***)&(*tokens)[i].globbed);
		i++;
	}
	free_void((void **)tokens, NULL);
}

static void	realloc_error(t_tokens *tokens, t_tokens *new_tokens,
			t_capacity *capacity, int i)
{
	error_handler(NULL, NULL, tokens->code, 1);
	free_tokens(&tokens, capacity);
	if (new_tokens != NULL)
	{
		capacity->max_size *= 2;
		while (++i < capacity->max_size)
			new_tokens[i].value = NULL;
		free_tokens(&new_tokens, capacity);
	}
}

t_tokens	*realloc_tokens_when_full(t_tokens *tokens,
		t_capacity *capacity, int i)
{
	t_tokens	*new_tokens;

	new_tokens = malloc(sizeof(t_tokens) * (capacity->max_size * 2));
	if (new_tokens == NULL)
		return (realloc_error(tokens, new_tokens, capacity, i), NULL);
	while (++i < capacity->max_size * 2)
	{
		if (i < capacity->current_size)
		{
			new_tokens[i].value = ft_strdup(tokens[i].value);
			if (new_tokens[i].value == NULL)
				return (realloc_error(tokens, new_tokens, capacity, i), NULL);
			new_tokens[i].globbed = NULL;
			new_tokens[i].type = tokens[i].type;
			new_tokens[i].code = tokens[i].code;
		}
		else
		{
			new_tokens[i].value = NULL;
			new_tokens[i].globbed = NULL;
		}
	}
	free_tokens(&tokens, capacity);
	capacity->max_size *= 2;
	return (new_tokens);
}

int	check_asterisk(int *int_array)
{
	int		i;
	char	c;

	i = 0;
	while (int_array[i])
	{
		c = (char)int_array[i];
		if (c == '*')
			return (0);
		i++;
	}
	return (1);
}
