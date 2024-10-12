/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 22:51:54 by upolat            #+#    #+#             */
/*   Updated: 2024/05/23 22:44:16 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_putchar_fd_int(char c, int fd)
{
	int	result;

	result = 0;
	result = write(fd, &c, 1);
	return (result);
}

static int	digit_counter(unsigned int un_i)
{
	int	digit;

	digit = 0;
	while (un_i > 0)
	{
		un_i = un_i / 10;
		digit++;
	}
	if (digit == 0)
		digit++;
	return (digit);
}

static int	ft_utoa_str_filler(char **str, int digit, unsigned int d)
{
	(*str)[digit] = '\0';
	while (digit > 0)
	{
		(*str)[--digit] = (d % 10) + '0';
		d = d / 10;
	}
	return (0);
}

char	*ft_utoa(unsigned int d)
{
	int				digit;
	char			*str;

	digit = digit_counter(d);
	str = malloc(sizeof(char) * (digit + 1));
	if (str == NULL)
		return (NULL);
	digit = ft_utoa_str_filler(&str, digit, d);
	while (str[digit])
	{
		if (ft_putchar_fd_int(str[digit], 1) == -1)
		{
			free(str);
			return (NULL);
		}
		digit++;
	}
	return (str);
}
