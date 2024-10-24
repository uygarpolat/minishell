/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup2.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hpirkola <hpirkola@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 14:43:21 by hpirkola          #+#    #+#             */
/*   Updated: 2024/10/24 14:59:59 by hpirkola         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ast.h"
/*
static int	count_size(char *str)
{
	int	count;

	count = 0;
	while (str[count])
		count++;
	return (count);
}
*/
char	**ft_strdup2(char **str)
{
	char	**word;
	int		i;
	//int	j;
	//int	count;

	i = 0;
	while (str[i])
		i++;
	word = malloc(sizeof(char *) * (i + 1));
	if (word == NULL)
		return (NULL);
	i = -1;
	while (str[++i])
	{
		/*count = 0;
		count = count_size(str[i]);
		word[i] = malloc(sizeof(char) * count + 1);
		if (!word[i])
			return (NULL);
		j = -1;
		while (word[i][++j])
			word[i][j] = str[i][j];
		word[i][j] = '\0';*/
		word[i] = ft_strdup(str[i]);
		if (!word[i])
			return (NULL);
	}
	word[i] = NULL;
	return (word);
}

