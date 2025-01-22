/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   var.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 10:52:48 by hpirkola          #+#    #+#             */
/*   Updated: 2025/01/22 19:42:24 by upolat           ###   ########.fr       */
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
		{
			i++;
			break ;
		}
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

char	**ch_var(char **envp, char *str)
{
	int	i;
	int	len;

	len = -1;
	while (str[++len])
	{
		if (str[len] == '=')
		{
			len++;
			break ;
		}
	}
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
