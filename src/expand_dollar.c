/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_dollar.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 18:33:55 by upolat            #+#    #+#             */
/*   Updated: 2024/11/15 23:45:56 by upolat           ###   ########.fr       */
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

int	handle_ambi_redir_in_dollar(t_arrays *a, char **str, char *var, int type)
{
	int	c;

	c = *(a->int_array_old - ft_strlen(*str) - 1);
	if (var == NULL || type == TOKEN_HEREDOC)
		return (0);
	if (ft_strchr(var, ' '))
	{
		if (identify_token(type))
		{
			if (type != TOKEN_HEREDOC && (((c >> 9) & 1) && !((c >> 8) & 1)))
			{
				ft_putstr_fd("minishell: $", 2);
				ft_putstr_fd(*str, 2);
				ft_putstr_fd(": ambiguous redirect\n", 2);
				free_void((void **)str, NULL);
				*a->code = 1;
				return (-1);
			}
		}
	}
	return (0);
}

char	*calloc_and_generate_env_variable_name(t_arrays *a)
{
	int		n;
	int		vlen;
	char	*str;

	vlen = 0;
	while ((ft_isalnum(a->int_array_old[vlen] & 0xFF) || (a->int_array_old[vlen]
				& 0xFF) == '_') && !((a->int_array_old[vlen] >> 9) & 1))
		vlen++;
	str = ft_calloc((vlen + 1), sizeof(char));
	if (str == NULL)
		return (error_handler(NULL, NULL, a->code, 1), NULL);
	n = -1;
	while (++n < vlen)
		str[n] = a->int_array_old[n];
	a->int_array_old += vlen;
	return (str);
}

int	str_combined(t_arrays *a, t_token_type type, int *len, int flag)
{
	char	*str;
	char	*var;

	str = calloc_and_generate_env_variable_name(a);
	if (str == NULL)
		return (-1);
	if (type == TOKEN_HEREDOC)
		var = str;
	else
		var = get_var(str, a->envp);
	if (handle_ambi_redir_in_dollar(a, &str, var, type) == -1)
		return (-1);
	if (var == NULL)
		return (free_void((void **)&str, NULL), 0);
	if (flag)
	{
		if (type == TOKEN_HEREDOC)
			*(a->int_array_new)++ = '$';
		while (*var)
			*(a->int_array_new)++ = *var++;
	}
	*len += (ft_strlen(var) + 1);
	free_void((void **)&str, NULL);
	return (0);
}

int	*malloc_array(t_arrays *a, int len, int flag)
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

int	when_q_mark_received(t_arrays *a, int *len, int flag)
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

void	when_non_dollar_received(t_arrays *a, int *len, int flag)
{
	(*len)++;
	if (flag)
	{
		*(a->int_array_new) = *(a->int_array_old);
		a->int_array_new++;
	}
	a->int_array_old++;
}

int	when_non_q_received(t_arrays *a, t_token_type type, int *len, int flag)
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
