/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_word_and_separator.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 00:23:17 by upolat            #+#    #+#             */
/*   Updated: 2024/11/16 01:22:20 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../library/libft/libft.h"
#include "../../includes/tokenizer.h"
#include "../../includes/ast.h"

int	malloc_individual_tokens(t_tokens *tokens, char **input,
		char *temp, t_capacity *capacity)
{
	tokens[capacity->current_size].value
		= malloc(sizeof(char) * (temp - *input + 1));
	if (tokens[capacity->current_size].value == NULL)
	{
		error_handler(NULL, NULL, tokens->code, 1);
		free_tokens(tokens, capacity);
		return (-1);
	}
	ft_strlcpy(tokens[capacity->current_size].value, *input, temp - *input + 1);
	capacity->current_size++;
	*input = temp;
	return (0);
}

int	handle_seperator(char **input, t_tokens *tokens, t_capacity *capacity)
{
	char			*temp;
	t_token_type	type;

	temp = *input;
	if (*temp == '<')
		assign_token_types(&temp, &type, TOKEN_REDIR_IN, TOKEN_HEREDOC);
	else if (*temp == '>')
		assign_token_types(&temp, &type, TOKEN_REDIR_OUT, TOKEN_APPEND);
	else if (*temp == '|')
		assign_token_types(&temp, &type, TOKEN_PIPE, TOKEN_OR);
	else if (*temp == '&' && *(temp + 1) == '&')
	{
		temp += 2;
		type = TOKEN_AND;
	}
	else if (*temp == '(')
		assign_token_types(&temp, &type, TOKEN_OPEN_PAREN, TOKEN_UNKNOWN);
	else
		assign_token_types(&temp, &type, TOKEN_CLOSE_PAREN, TOKEN_UNKNOWN);
	tokens[capacity->current_size].type = type;
	if (malloc_individual_tokens(tokens, input, temp, capacity))
		return (-1);
	return (0);
}

char	*skip_a_char(char *str, char c)
{
	char	*temp;
	int		flag;

	temp = NULL;
	flag = 0;
	str++;
	while (*str && *str != c)
	{
		if (ft_strchr(" \n\t<>|&()\"'", *str) && !flag)
		{
			flag = 1;
			temp = str - 1;
		}
		str++;
	}
	if (*str)
		return (str);
	else if (temp != NULL)
		return (temp);
	return (str);
}

int	skip_quotes(char **temp)
{
	if (ft_strchr(" \n\t<>|&()\"'", **temp))
	{
		if (**temp == '"')
			*temp = skip_a_char(*temp, '"');
		else if (**temp == '\'')
			*temp = skip_a_char(*temp, '\'');
		else
			return (0);
	}
	return (1);
}

int	handle_word(char **input, t_tokens *tokens, t_capacity *capacity)
{
	char	*temp;

	temp = *input;
	if (!*temp)
		return (0);
	while (*temp)
	{
		while (*temp && skip_quotes(&temp))
		{
			if (*temp)
				temp++;
		}
		if (*temp == '&')
		{
			if (*(temp + 1) == '&')
				break ;
			temp++;
		}
		else
			break ;
	}
	tokens[capacity->current_size].type = TOKEN_WORD;
	if (malloc_individual_tokens(tokens, input, temp, capacity))
		return (-1);
	return (0);
}
