/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup2.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hpirkola <hpirkola@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 14:43:21 by hpirkola          #+#    #+#             */
/*   Updated: 2024/10/28 17:12:43 by hpirkola         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ast.h"

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

