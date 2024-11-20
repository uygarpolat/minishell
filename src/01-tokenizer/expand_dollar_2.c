/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_dollar_2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 00:47:57 by upolat            #+#    #+#             */
/*   Updated: 2024/11/17 18:58:04 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/tokenizer.h"

int	identify_token(t_token_type type);

static char	*get_var(char *str, char **envp)
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

static int	handle_ambi_redir_in_dollar(t_arrays *a, char **str,
				char *var, int type)
{
	int	c;

	c = *(a->int_array_old - ft_strlen(*str) - 1);
	if (type == TOKEN_HEREDOC)
		return (0);
	if (var == NULL || ft_strchr(var, ' '))
	{
		if (identify_token(type))
		{
			if (((c >> 9) & 1) && !((c >> 8) & 1))
			{
				ft_putstr_fd("minishell: $", 2);
				ft_putstr_fd(*str, 2);
				ft_putstr_fd(": ambiguous redirect\n", 2);
				free_void((void **)str, NULL);
				*a->code = 1;
				return (-1);
			}
			else if (var == NULL)
				return (error_handler("", "No such file or directory",
						a->code, 1), -1);
		}
	}
	return (0);
}

static char	*calloc_and_generate_env_variable_name(t_arrays *a)
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
