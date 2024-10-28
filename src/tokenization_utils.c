/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenization_utils.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 18:47:24 by upolat            #+#    #+#             */
/*   Updated: 2024/10/27 18:53:25 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "../../library/libft/libft.h"
#include "../includes/tokenizer.h"
#include "../includes/ast.h"

char	*ft_strjoin_free(char *s1, char *s2)
{
	int		i;
	int		j;
	char	*str;

	i = 0;
	str = malloc(sizeof(char) * (ft_strlen(s1) + ft_strlen(s2) + 1));
	if (str == NULL)
		return (NULL);
	i = 0;
	while (s1[i])
	{
		str[i] = s1[i];
		i++;
	}
	j = 0;
	while (s2[j])
	{
		str[i] = s2[j];
		i++;
		j++;
	}
	str[i] = '\0';
	free(s1);
	s1 = NULL;
	return (str);
}
