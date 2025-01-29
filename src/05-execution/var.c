/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   var.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 10:52:48 by hpirkola          #+#    #+#             */
/*   Updated: 2025/01/29 12:19:10 by hpirkola         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/ast.h"

char	*get_var(char **envp, char *key)
{
	while (*envp)
	{
		if (!ft_strncmp(*envp, key, ft_strlen(key)))
			return (*envp + ft_strlen(key));
		envp++;
	}
	return (NULL);
}

int	var_exists(char **envp, char *str)
{
	int		i;
	int		j;
	char	*temp;

	i = -1;
	while (str[++i])
	{
		if (str[i] == '=')
			break ;
	}
	temp = malloc(sizeof(char) * (i + 1));
	j = -1;
	while (++j < i)
		temp[j] = str[j];
	temp[j] = '\0';
	if (get_var(envp, temp) == NULL)
	{
		free(temp);
		return (0);
	}
	free(temp);
	return (1);
}

static int	count_len(char *str)
{
	int	len;
	int	flag;

	len = -1;
	flag = 0;
	while (str[++len])
	{
		if (str[len] == '=')
		{
			flag = 1;
			len++;
			break ;
		}
	}
	if (flag)
		return (len);
	return (-1);
}

char	**ch_var(char **envp, char *str)
{
	int	i;
	int	len;

	len = count_len(str);
	if (len == -1)
		return (envp);
	i = -1;
	while (envp[++i])
	{
		if (!ft_strncmp(envp[i], str, len))
		{
			free(envp[i]);
			envp[i] = ft_strdup(str);
			if (!envp[i])
				return (free_2d_array((void ***)&envp), NULL);
			break ;
		}
	}
	return (envp);
}
