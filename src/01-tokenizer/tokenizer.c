/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 11:16:11 by upolat            #+#    #+#             */
/*   Updated: 2024/11/16 01:13:27 by upolat           ###   ########.fr       */
/*   Updated: 2024/11/07 12:36:19 by hpirkola         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../library/libft/libft.h"
#include "../../includes/tokenizer.h"
#include "../../includes/ast.h"

int	init_tokenizer(t_tokens **tokens, t_capacity *capacity, int *code)
{
	capacity->max_size = 1;
	capacity->current_size = 0;
	*tokens = malloc(sizeof(t_tokens) * capacity->max_size);
	if (*tokens == NULL)
		return (error_handler(NULL, NULL, (*tokens)->code, 1), -1);
	(*tokens)->globbed = NULL;
	(*tokens)->code = code;
	return (0);
}

t_tokens	*ft_tokenizer(char *input, t_capacity *capacity,
				char **envp, int *code)
{
	t_tokens	*tokens;
	int			error_code;

	tokens = NULL;
	if (init_tokenizer(&tokens, capacity, code) == -1)
		return (NULL);
	while (*input)
	{
		while (ft_strchr(" \t\n", *input) && *input)
			input++;
		if (capacity->max_size <= capacity->current_size)
			tokens = realloc_tokens_when_full(tokens, capacity, -1);
		if (tokens == NULL)
			return (NULL);
		if (*input && is_seperator(*input, *(input + 1)))
			error_code = handle_seperator(&input, tokens, capacity);
		else if (*input)
			error_code = handle_word(&input, tokens, capacity);
		if (error_code == -1)
			return (NULL);
	}
	//print_tokens(tokens, capacity);
	if (tokens_error_checker(tokens, capacity) == -1
		|| handle_expansion_and_wildcard(tokens, capacity, envp) == -1)
		return ((t_tokens *)free_tokens(tokens, capacity));
	return (tokens);
}
