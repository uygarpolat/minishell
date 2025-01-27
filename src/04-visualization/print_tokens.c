/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_tokens.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 00:32:25 by upolat            #+#    #+#             */
/*   Updated: 2025/01/27 23:21:19 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/tokenizer.h"

void	print_tokens(t_tokens *tokens, t_capacity *capacity, int flag)
{
	int		i;
	char	*token_type_str[12];

	if (!flag)
		return ;
	token_type_str[0] = "TOKEN_WORD";
	token_type_str[1] = "TOKEN_REDIR_IN";
	token_type_str[2] = "TOKEN_REDIR_OUT";
	token_type_str[3] = "TOKEN_APPEND";
	token_type_str[4] = "TOKEN_HEREDOC";
	token_type_str[5] = "TOKEN_AND";
	token_type_str[6] = "TOKEN_OR";
	token_type_str[7] = "TOKEN_OPEN_PAREN";
	token_type_str[8] = "TOKEN_CLOSE_PAREN";
	token_type_str[9] = "TOKEN_PIPE";
	token_type_str[10] = "TOKEN_UNKNOWN";
	token_type_str[11] = "NUM_TYPES";
	i = 0;
	while (i < capacity->current_size)
	{
		printf("token %d: %-20s type: %s\n",
			i, tokens[i].value, token_type_str[tokens[i].type]);
		i++;
	}
	printf("----------\n");
}
