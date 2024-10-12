/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 01:51:36 by upolat            #+#    #+#             */
/*   Updated: 2024/04/24 09:00:39 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	size_t		i;
	char		*destchar;
	const char	*srcchar;

	if (dest == NULL && src == NULL)
		return (NULL);
	i = 0;
	destchar = (char *)dest;
	srcchar = (const char *)src;
	while (i < n)
	{
		destchar[i] = srcchar[i];
		i++;
	}
	return (dest);
}
