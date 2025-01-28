/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_and_grammar.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 00:07:15 by upolat            #+#    #+#             */
/*   Updated: 2025/01/28 16:54:53 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/tokenizer.h"

int		identify_token(t_token_type type);
void	syntax_error_near(t_tokens *tokens, int loc);
int		find_matching_paren(t_tokens *tokens, int start, int end);

static int	tokens_grammar(t_tokens *tokens, t_capacity *capacity, int i)
{
	while (++i < capacity->current_size - 1)
	{
		if ((identify_token(tokens[i].type) && tokens[i + 1].type
				!= TOKEN_WORD)
			|| (tokens[i].type == TOKEN_WORD && tokens[i + 1].type
				== TOKEN_OPEN_PAREN) || (tokens[i].type == TOKEN_PIPE
				&& !identify_token(tokens[i + 1].type) && tokens[i + 1].type
				!= TOKEN_WORD && tokens[i + 1].type != TOKEN_OPEN_PAREN)
			|| (tokens[i].type
				== TOKEN_AND && !identify_token(tokens[i + 1].type)
				&& tokens[i + 1].type != TOKEN_WORD && tokens[i + 1].type
				!= TOKEN_OPEN_PAREN) || (tokens[i].type == TOKEN_OR
				&& !identify_token(tokens[i + 1].type) && tokens[i + 1].type
				!= TOKEN_WORD && tokens[i + 1].type
				!= TOKEN_OPEN_PAREN) || (tokens[i].type == TOKEN_OPEN_PAREN
				&& !identify_token(tokens[i + 1].type) && tokens[i + 1].type
				!= TOKEN_WORD && tokens[i + 1].type != TOKEN_OPEN_PAREN)
			|| (tokens[i].type == TOKEN_CLOSE_PAREN
				&& tokens[i + 1].type != TOKEN_AND && tokens[i + 1].type
				!= TOKEN_OR && tokens[i + 1].type != TOKEN_PIPE
				&& tokens[i + 1].type != TOKEN_CLOSE_PAREN))
			return (syntax_error_near(tokens, i + 1), -1);
	}
	return (0);
}

int	tokens_error_checker(t_tokens *tokens, t_capacity *capacity, int i, int k)
{
	print_tokens(tokens, capacity, 0);
	if (tokens[0].type != TOKEN_WORD && !identify_token(tokens[0].type)
		&& tokens[0].type != TOKEN_OPEN_PAREN)
		return (syntax_error_near(tokens, 0), -1);
	if (tokens_grammar(tokens, capacity, -1) == -1)
		return (-1);
	if (tokens[capacity->current_size - 1].type != TOKEN_WORD
		&& tokens[capacity->current_size - 1].type != TOKEN_CLOSE_PAREN)
		return (syntax_error_near(tokens, -1), -1);
	i = 0;
	k = 0;
	while (i < capacity->current_size)
	{
		if (tokens[i].type == TOKEN_AND || tokens[i].type == TOKEN_OR)
			return (syntax_error_near(tokens, i), -1);
		if (tokens[i].type == TOKEN_OPEN_PAREN
			|| tokens[i].type == TOKEN_CLOSE_PAREN)
			k = find_matching_paren(tokens, i, capacity->current_size - 1);
		if (k < 0)
			return (syntax_error_near(tokens, i), -1);
		if (k > i)
			i = k;
		i++;
	}
	return (0);
}

char	*check_match_count(t_tokens *tokens, int *int_array, int match_count)
{
	if (!match_count)
		return (back_to_char(tokens, int_array));
	return (NULL);
}
