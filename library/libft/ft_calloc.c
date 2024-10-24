/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/13 03:33:24 by upolat            #+#    #+#             */
/*   Updated: 2024/04/23 18:20:19 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_calloc(size_t count, size_t size)
{
	void	*voidstr;
	char	*charstr;
	size_t	memory;
	size_t	i;

	memory = count * size;
	if (count != 0 && SIZE_MAX / count < size)
		return (NULL);
	voidstr = malloc(memory);
	if (voidstr == NULL)
		return (NULL);
	i = 0;
	charstr = (char *)voidstr;
	while (i < memory)
	{
		charstr[i] = 0;
		i++;
	}
	return (voidstr);
}
