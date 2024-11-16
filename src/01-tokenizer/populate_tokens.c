/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   populate_tokens.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 00:20:22 by upolat            #+#    #+#             */
/*   Updated: 2024/11/16 14:10:43 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/tokenizer.h"

static void	assign_dollar(char *str, int *int_array, t_quote *q, int *m)
{
	if (q->single_q_count % 2 != 1)
	{
		if (ft_isalnum(*(str + 1)) || (*(str + 1) == '_')
			|| (*(str + 1) == '?'))
		{
			int_array[*m] = encode_char_with_flag(*str, 9);
			if (q->double_q_count % 2 == 1)
				int_array[*m] = encode_char_with_flag(int_array[*m], 8);
		}
		else if ((*(str + 1) == '"' || *(str + 1) == '\'')
			&& q->double_q_count % 2 != 1)
			return ;
		else
			int_array[*m] = *str;
	}
	else
		int_array[*m] = *str;
	(*m)++;
}

static void	assign_asterisk(char *str, int *int_array, t_quote *q, int *m)
{
	if (q->single_q_count % 2 == 0 && q->double_q_count % 2 == 0)
		int_array[*m] = encode_char_with_flag(*str, 9);
	else
		int_array[*m] = *str;
	(*m)++;
}

static void	assign_quote(char **str, int *int_array, t_quote *q, int *m)
{
	if (**str == '"')
	{
		if (q->double_q_count % 2 == 0)
			q->double_first_unclosed = *str;
		else
			q->double_first_unclosed = q->str_initial;
		q->double_q_count++;
	}
	else
	{
		if (q->single_q_count % 2 == 0)
			q->single_first_unclosed = *str;
		else
			q->single_first_unclosed = q->str_initial;
		q->single_q_count++;
	}
	if (ft_isalnum(*(*str + 1) & 0xFF) || (*(*str + 1) & 0xFF) == '_')
	{
		int_array[*m] = encode_char_with_flag(*(*str + 1), 9);
		(*m)++;
		if (*(*str + 1) & 0xFF)
			(*str)++;
	}
}

static void	init_populate_tokens(t_quote *q, char *str)
{
	q->single_q_count = 0;
	q->double_q_count = 0;
	q->single_first_unclosed = &str[ft_strlen(str)];
	q->double_first_unclosed = &str[ft_strlen(str)];
	q->str_initial = &str[ft_strlen(str)];
}

int	populate_tokens(char *str, int *code, int *int_array, int m)
{
	t_quote	q;

	init_populate_tokens(&q, str);
	while (*str)
	{
		if (*str == '"' && q.single_q_count % 2 == 0)
			assign_quote(&str, int_array, &q, &m);
		else if (*str == '\'' && q.double_q_count % 2 == 0)
			assign_quote(&str, int_array, &q, &m);
		else if (*str == '$')
			assign_dollar(str, int_array, &q, &m);
		else if (*str == '*')
			assign_asterisk(str, int_array, &q, &m);
		else
			int_array[m++] = *str;
		str++;
	}
	if (q.double_first_unclosed < q.single_first_unclosed)
		return (error_handler(NULL, "unexpected EOF "
				"while looking for matching `\"'", code, 2), -1);
	else if (q.double_first_unclosed > q.single_first_unclosed)
		return (error_handler(NULL, "unexpected EOF "
				"while looking for matching `''", code, 2), -1);
	return (0);
}
