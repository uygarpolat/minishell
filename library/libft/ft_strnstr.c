/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/13 16:02:43 by upolat            #+#    #+#             */
/*   Updated: 2024/04/23 18:19:43 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strnstr(const char *haystack, const char *needle, size_t len)
{
	size_t		j;
	size_t		needle_len;
	const char	*temp_haystack;

	temp_haystack = haystack;
	needle_len = ft_strlen(needle);
	if (needle_len == 0)
		return ((char *)temp_haystack);
	while (*haystack && len >= needle_len)
	{
		j = 0;
		while (!ft_strncmp(haystack, &needle[j], 1))
		{
			j++;
			haystack++;
			if (j == needle_len)
				return ((char *)(haystack - needle_len));
		}
		if (j != 0)
			haystack -= j;
		haystack++;
		len--;
	}
	return (NULL);
}
