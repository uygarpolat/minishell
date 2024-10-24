/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_base.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/05 10:11:46 by upolat            #+#    #+#             */
/*   Updated: 2024/05/23 22:43:14 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	base_checker(char *base)
{
	int	i;

	i = 0;
	while (base[i] != 0)
	{
		if (base[i] == '+' || base[i] == '-')
			return (0);
		i++;
	}
	return (i);
}

int	ft_putnbr_base_unsigned(uintptr_t nb, char *base)
{
	unsigned int	len;
	int				total_length;
	int				recursive_result;

	total_length = 0;
	len = base_checker(base);
	if (nb < (uintptr_t)len)
	{
		if (ft_putchar_fd_int(base[nb], 1) == -1)
			return (-1);
		total_length++;
	}
	else
	{
		recursive_result = ft_putnbr_base_unsigned(nb / len, base);
		if (recursive_result == -1)
			return (-1);
		total_length += recursive_result;
		if (ft_putchar_fd_int(base[nb % len], 1) == -1)
			return (-1);
		total_length++;
	}
	return (total_length);
}
