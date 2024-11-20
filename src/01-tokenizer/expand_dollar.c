/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_dollar.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 18:33:55 by upolat            #+#    #+#             */
/*   Updated: 2024/11/16 14:17:35 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/tokenizer.h"

static int	*malloc_array(t_arrays *a, int len, int flag)
{
	int	*arr;

	arr = NULL;
	if (!flag)
	{
		arr = ft_calloc((len + 1), sizeof(int));
		if (arr == NULL)
			return (error_handler(NULL, NULL, a->code, 1), NULL);
		return (arr);
	}
	return (arr);
}

static int	when_q_mark_received(t_arrays *a, int *len, int flag)
{
	int		num;
	char	*str_num;

	str_num = ft_itoa(*a->code);
	if (str_num == NULL)
		return (error_handler(NULL, NULL, a->code, 1), -1);
	num = ft_strlen(str_num);
	*len += num;
	a->int_array_old += 2;
	if (flag)
	{
		while (*str_num)
			*(a->int_array_new)++ = *str_num++;
		str_num -= num;
	}
	free_void((void **)&str_num, NULL);
	return (0);
}

static void	when_non_dollar_received(t_arrays *a, int *len, int flag)
{
	(*len)++;
	if (flag)
	{
		*(a->int_array_new) = *(a->int_array_old);
		a->int_array_new++;
	}
	a->int_array_old++;
}

static int	when_non_q_received(t_arrays *a, t_token_type type,
				int *len, int flag)
{
	a->int_array_old++;
	if (str_combined(a, type, len, flag) == -1)
		return (-1);
	return (0);
}

int	*ultimate_dollar_expansion(t_arrays *a,
		t_token_type type, int flag, int len)
{
	while (*(a->int_array_old))
	{
		if (((*(a->int_array_old) & 0xFF) == '$')
			&& ((*(a->int_array_old) >> 9) & 1) && *(a->int_array_old + 1))
		{
			if (*(a->int_array_old + 1) == '?')
			{
				if (when_q_mark_received(a, &len, flag))
					return (NULL);
			}
			else
			{
				if (when_non_q_received(a, type, &len, flag))
					return (NULL);
			}
		}
		else
			when_non_dollar_received(a, &len, flag);
	}
	if (!flag)
		return (malloc_array(a, len, flag));
	else
		return (a->int_array_old);
}
