/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_dollar.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 18:33:55 by upolat            #+#    #+#             */
/*   Updated: 2024/11/14 12:53:18 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../library/libft/libft.h"
#include "../includes/tokenizer.h"
#include "../includes/ast.h"
#include "termios.h"

char	*get_var(char *str, char **envp)
{
	if (!envp)
		return (NULL);
	while (*envp)
	{
		if (ft_strncmp(*envp, str, ft_strlen(str)) == 0
			&& *(*envp + ft_strlen(str)) == '=')
			return (*envp + ft_strlen(str) + 1);
		envp++;
	}
	return (NULL);
}

int	str_combined(int **int_array_old, int **int_array_new, char **envp, int flg)
{
	int		n;
	char	*str;
	char	*var;
	int		len;

	len = 0;
	while ((ft_isalnum((*int_array_old)[len] & 0xFF) || ((*int_array_old)[len]
		& 0xFF) == '_') && !(((*int_array_old)[len] >> 9) & 1))
		len++;
	str = ft_calloc((len + 1), sizeof(char));
	if (str == NULL)
		return (error_handler(NULL, NULL), -1);
	n = -1;
	while (++n < len)
		str[n] = (*int_array_old)[n];
	*int_array_old = *int_array_old + len;
	var = get_var(str, envp);
	free_void((void **)&str, NULL);
	if (var == NULL)
		return (0);
	len = ft_strlen(var);
	while (flg && *var)
		*(*int_array_new)++ = *var++;
	return (len);
}

int	when_q_mark_received(t_arrays **a, int *len, int flag)
{
	int		num;
	char	*str_num;

	str_num = ft_itoa((*a)->code);
	if (str_num == NULL)
		return (error_handler(NULL, NULL), -1);
	num = ft_strlen(str_num);
	*len = *len + num;
	(*a)->int_array_old += 2;
	if (flag)
	{
		while (*str_num)
			*((*a)->int_array_new)++ = *str_num++;
		str_num = str_num - num;
	}
	free_void((void **)&str_num, NULL);
	return (0);
}

int	*malloc_array(int len, int flag)
{
	int	*arr;

	arr = NULL;
	if (!flag)
	{	
		arr = ft_calloc((len + 1), sizeof(int));
		if (arr == NULL)
			return (error_handler(NULL, NULL), NULL);
		return (arr);
	}
	return (0); // This is probably wrong
}

void	when_non_dollar_received(t_arrays **a, int *len, int flag)
{
	(*len)++;
	if (flag)
	{	
		*((*a)->int_array_new) = *((*a)->int_array_old);
		((*a)->int_array_new)++;
	}
	((*a)->int_array_old)++;
}

int	when_non_q_received(t_arrays **a, int *len, int flag)
{
	int	num;

	(*((*a)->int_array_old))++;
	num = str_combined(&((*a)->int_array_old), &((*a)->int_array_new), (*a)->envp, flag);
	if (num == -1)
		return (-1);
	*len = *len + num;
	return (0);
}

int	*ultimate_dollar_expansion(t_arrays *a, int flag)
{
	int	len;

	len = 0;
	while (*a->int_array_old)
	{
		if (((*a->int_array_old & 0xFF) == '$')
			&& ((*a->int_array_old >> 9) & 1) && *(a->int_array_old + 1))
		{
			if (*(a->int_array_old + 1) == '?')
			{
				if (when_q_mark_received(&a, &len, flag))
					return (NULL);
			}
			else
			{
				if (when_non_q_received(&a, &len, flag))
					return (NULL);
			}
		}
		else
			when_non_dollar_received(&a, &len, flag);
	}
	return (malloc_array(len, flag));
}
