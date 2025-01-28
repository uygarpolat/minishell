/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion_and_wildcard.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 00:10:10 by upolat            #+#    #+#             */
/*   Updated: 2025/01/28 08:44:03 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/tokenizer.h"

int	identify_token(t_token_type type);

static void	init_arrays(t_tokens *tokens, t_arrays *a, char **envp)
{
	a->int_array_new = NULL;
	a->int_array_old = NULL;
	a->int_array_new_start = NULL;
	a->int_array_old_start = NULL;
	a->envp = envp;
	a->code = tokens->code;
}

static void	free_int_arrays(t_arrays *a)
{
	free_void((void **)&a->int_array_old_start, NULL);
	free_void((void **)&a->int_array_new_start, NULL);
}

static int	calloc_and_populate(t_tokens *tokens,
		t_arrays *a, t_token_type *type, int i)
{
	if (i < 1)
		*type = TOKEN_UNKNOWN;
	else if (identify_token(tokens[i - 1].type))
		*type = tokens[i - 1].type;
	a->int_array_old = ft_calloc((ft_strlen(tokens[i].value)
				+ 1), sizeof(int));
	if (a->int_array_old == NULL)
		return (error_handler(NULL, NULL, tokens->code, 1), -1);
	if (populate_tokens(tokens[i].value, tokens->code, a->int_array_old, 0))
		return (free_void((void **)&a->int_array_old, NULL), -1);
	a->int_array_old_start = a->int_array_old;
	return (0);
}

int	handle_expansion_and_wildcard_utils(t_tokens *tokens, t_arrays *a, int i)
{
	a->int_array_new = a->int_array_new_start;
	if ((char)a->int_array_new[0] == '\0'
		&& ft_strchr(tokens[i].value, '$') && tokens[i].value[0] != '"')
	{
		free_void((void **)&tokens[i].value, NULL);
		free_int_arrays(a);
		return (1);
	}
	// if (!check_char_in_intarray(a->int_array_new, ' ')
	// 	&& ft_strchr(tokens[i].value, '$') && tokens[i].value[0] != '"')
	// 	tokens[i].flag = 1;
	free_void((void **)&tokens[i].value, NULL);
	return (0);
}

int	handle_expansion_and_wildcard(t_tokens *tokens,
		t_capacity *capacity, char **envp)
{
	int				i;
	t_token_type	type;
	t_arrays		a;

	init_arrays(tokens, &a, envp);
	i = -1;
	while (++i < capacity->current_size)
	{
		if (calloc_and_populate(tokens, &a, &type, i) == -1)
			return (-1);
		a.int_array_new = ultimate_dollar_expansion(&a, type, 0, 0);
		if (a.int_array_new == NULL)
			return (free_int_arrays(&a), -1);
		a.int_array_new_start = a.int_array_new;
		a.int_array_old = a.int_array_old_start;
		if (ultimate_dollar_expansion(&a, type, 1, 0) == NULL)
			return (free_int_arrays(&a), -1);
		if (handle_expansion_and_wildcard_utils(tokens, &a, i))
			continue ;
		tokens[i].value = expand_wildcard(a.int_array_new, tokens, i, 0);
		free_int_arrays(&a);
		if (tokens[i].value == NULL)
			return (-1);
	}
	return (0);
}
