/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenization_utils.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 18:47:24 by upolat            #+#    #+#             */
/*   Updated: 2024/11/10 16:19:09 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "../../library/libft/libft.h"
#include "../includes/tokenizer.h"
#include "../includes/ast.h"

void	error_handler(char *cause_str, char *error_str)
{
	ft_putstr_fd("minishell: ", 2);
	if (cause_str)
	{
		ft_putstr_fd(cause_str, 2);
		ft_putstr_fd(": ", 2);
	}
	if (error_str)
		ft_putstr_fd(error_str, 2);
	else
		ft_putstr_fd(strerror(errno), 2);
	ft_putstr_fd("\n", 2);
}

char	*ft_strjoin_free(char *s1, char *s2)
{
	int		i;
	int		j;
	char	*str;

	str = malloc(sizeof(char) * (ft_strlen(s1) + ft_strlen(s2) + 1));
	if (str == NULL)
		return (free_void((void **)&s1, NULL));
	i = -1;
	while (s1[++i])
		str[i] = s1[i];
	j = 0;
	while (s2[j])
		str[i++] = s2[j++];
	str[i] = '\0';
	free_void((void **)&s1, NULL);
	return (str);
}

char	*back_to_char(int *int_array)
{
	int		i;
	char	*str;

	i = 0;
	while (int_array[i])
		i++;
	str = malloc(sizeof(char) * (i + 1));
	if (str == NULL)
		return (error_handler(NULL, NULL), NULL);
	str[i] = '\0';
	while (--i >= 0)
		str[i] = (char)int_array[i];
	return (str);
}
