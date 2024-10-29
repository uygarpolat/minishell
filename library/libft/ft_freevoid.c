/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_freevoid.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/29 22:27:13 by upolat            #+#    #+#             */
/*   Updated: 2024/10/29 22:31:56 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	free_2d_array(void ***arr)
{
	int	i;

	i = 0;
	if (arr == NULL || *arr == NULL)
		return ;
	while ((*arr)[i])
	{
		free((*arr)[i]);
		(*arr)[i] = NULL;
		i++;
	}
	free(*arr);
	*arr = NULL;
}

void	*free_void(void **var, void *return_value)
{
	if (var && *var)
	{
		free(*var);
		*var = NULL;
	}
	return (return_value);
}
