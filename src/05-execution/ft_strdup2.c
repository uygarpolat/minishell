/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup2.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 14:43:21 by hpirkola          #+#    #+#             */
/*   Updated: 2025/01/22 21:40:48 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/ast.h"

char	**ft_strdup3(char **str, char *dir)
{
	char	**word;
	char	*new_dir;
	int		i;

	i = 0;
	while (str[i])
		i++;
	word = malloc(sizeof(char *) * (i + 1));
	if (word == NULL)
		return (NULL);
	i = -1;
	while (str[++i])
	{
		if (!ft_strncmp(str[i], "PWD=", 4))
		{
			new_dir = ft_strjoin("PWD=", dir);
			word[i] = new_dir;
		}
		else
			word[i] = ft_strdup(str[i]);
		if (!word[i])
			return (NULL);
	}
	word[i] = NULL;
	return (word);
}

char	**ft_strdup2(char **str)
{
	char	**word;
	int		i;

	i = 0;
	while (str[i])
		i++;
	word = malloc(sizeof(char *) * (i + 1));
	if (word == NULL)
		return (NULL);
	i = -1;
	while (str[++i])
	{
		word[i] = ft_strdup(str[i]);
		if (!word[i])
		{
			i = -1;
			while (word[++i])
				free(word[i]);
			free(word);
			return (NULL);
		}
	}
	word[i] = NULL;
	return (word);
}
