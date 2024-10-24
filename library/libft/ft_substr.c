/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/10 01:04:03 by upolat            #+#    #+#             */
/*   Updated: 2024/04/23 18:20:48 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	size_t	sublen;
	char	*substr;

	sublen = 0;
	if (start >= ft_strlen(s))
		start = ft_strlen(s);
	while (s[start] != '\0' && sublen < len)
	{
		start++;
		sublen++;
	}
	substr = malloc(sizeof(char) * (sublen + 1));
	if (substr == NULL)
		return (NULL);
	start = start - sublen;
	sublen = 0;
	while (s[start] != '\0' && sublen < len)
	{
		substr[sublen] = s[start];
		sublen++;
		start++;
	}
	substr[sublen] = '\0';
	return (substr);
}
