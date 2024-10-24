/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/20 11:24:58 by upolat            #+#    #+#             */
/*   Updated: 2024/04/23 18:16:32 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dest, const char *src, size_t size)
{
	size_t	i;
	size_t	k;
	size_t	j;

	i = 0;
	k = 0;
	j = 0;
	while (dest[i] != 0)
		i++;
	while (src[j] != 0)
		j++;
	if (size < i)
		return (j + size);
	while (src[k] != 0 && i + 1 < size)
	{
		dest[i] = src[k];
		i++;
		k++;
	}
	dest[i] = 0;
	return (j + i - k);
}
