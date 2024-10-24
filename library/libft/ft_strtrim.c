/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/10 02:26:22 by upolat            #+#    #+#             */
/*   Updated: 2024/04/23 18:21:29 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strtrim(char const *s1, char const *set)
{
	int	len;

	while (*s1 && ft_strchr(set, *s1))
		s1++;
	len = ft_strlen((char *) s1);
	while (len && ft_strchr(set, s1[len - 1]))
		len--;
	return ((char *) ft_substr(s1, 0, len));
}
