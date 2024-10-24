/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/13 01:31:12 by upolat            #+#    #+#             */
/*   Updated: 2024/04/22 14:39:53 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *dst, const void *src, size_t len)
{
	size_t		i;
	char		*chardst;
	const char	*charsrc;

	if (dst == NULL && src == NULL)
		return (NULL);
	chardst = (char *)dst;
	charsrc = (const char *)src;
	if (chardst > charsrc)
	{
		i = len;
		while (i-- > 0)
			chardst[i] = charsrc[i];
	}
	else
	{
		i = 0;
		while (i++ < len)
			chardst[i - 1] = charsrc[i - 1];
	}
	return (dst);
}
