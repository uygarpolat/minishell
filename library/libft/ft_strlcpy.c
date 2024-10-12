/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/08 18:05:28 by upolat            #+#    #+#             */
/*   Updated: 2024/04/23 18:16:16 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcpy(char *destination, const char *source, size_t size)
{
	size_t	i;
	size_t	counter;

	counter = 0;
	while (source[counter])
		counter++;
	i = 0;
	while (source[i] != '\0' && i + 1 < size)
	{
		destination[i] = source[i];
		i++;
	}
	if (size > 0)
		destination[i] = '\0';
	return (counter);
}
