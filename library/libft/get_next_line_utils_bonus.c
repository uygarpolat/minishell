/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils_bonus.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/01 14:16:57 by upolat            #+#    #+#             */
/*   Updated: 2024/05/21 12:29:22 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strchr_plus(const char *str, int c)
{
	while (*str)
	{
		if (*str == (char)c)
			return ((char *)(str + 1));
		str++;
	}
	if ((char)c == '\0')
		return ((char *)str);
	return (0);
}

char	*ft_str_tillchar(char *str, int c)
{
	int		i;
	int		n;
	char	*str_return;

	i = 0;
	n = 0;
	while (str[i] != (char)c)
		i++;
	n = i + 1;
	str_return = malloc(sizeof(char) * (n + 1));
	if (str_return == NULL)
		return (NULL);
	i = 0;
	while (i < n)
	{
		str_return[i] = str[i];
		i++;
	}
	str_return[i] = '\0';
	return (str_return);
}
