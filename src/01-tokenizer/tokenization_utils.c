/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenization_utils.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 18:47:24 by upolat            #+#    #+#             */
/*   Updated: 2024/11/16 13:24:50 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/tokenizer.h"

int	is_seperator(char c, char c_plus_one)
{
	if (c == '<' || c == '>' || (c == '&' && c_plus_one == '&')
		|| c == '|' || c == '(' || c == ')')
		return (1);
	return (0);
}

void	assign_token_types(char **temp, t_token_type *type,
			t_token_type type1, t_token_type type2)
{
	(*temp)++;
	if (type2 != TOKEN_UNKNOWN && **temp == *(*temp - 1))
	{
		(*temp)++;
		*type = type2;
	}
	else
		*type = type1;
}

int	encode_char_with_flag(int c, int shift_amount)
{
	int	encoded_value;

	encoded_value = (1 << shift_amount) | c;
	return (encoded_value);
}

void	error_handler(char *cause_str, char *error_str,
			int *code_address, int exit_code)
{
	ft_putstr_fd("minishell: ", 2);
	if (cause_str)
	{
		ft_putstr_fd(cause_str, 2);
		ft_putstr_fd(": ", 2);
	}
	if (error_str)
		ft_putstr_fd(error_str, 2);
	else
		ft_putstr_fd(strerror(errno), 2);
	ft_putstr_fd("\n", 2);
	*code_address = exit_code;
}

char	*back_to_char(t_tokens *tokens, int *int_array)
{
	int		i;
	char	*str;

	i = 0;
	while (int_array[i])
		i++;
	str = malloc(sizeof(char) * (i + 1));
	if (str == NULL)
		return (error_handler(NULL, NULL, tokens->code, 1), NULL);
	str[i] = '\0';
	while (--i >= 0)
		str[i] = (char)int_array[i];
	return (str);
}
