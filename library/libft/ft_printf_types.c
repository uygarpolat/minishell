/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_types.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/05 13:12:00 by upolat            #+#    #+#             */
/*   Updated: 2024/05/23 22:43:33 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	print_d_i(int d)
{
	char	*nbr;
	int		result;
	int		i;

	i = 0;
	result = 0;
	nbr = ft_itoa(d);
	if (nbr == NULL)
		return (-1);
	result = ft_strlen(nbr);
	while (nbr[i])
	{
		if (ft_putchar_fd_int(nbr[i], 1) == -1)
		{
			free(nbr);
			return (-1);
		}
		i++;
	}
	free(nbr);
	return (result);
}

int	print_s(char *s)
{
	int		result;
	char	*null;
	int		i;

	i = 0;
	null = "(null)";
	if (s == NULL)
	{
		while (null[i])
		{
			if (ft_putchar_fd_int(null[i], 1) == -1)
				return (-1);
			i++;
		}
		return (6);
	}
	i = 0;
	result = ft_strlen(s);
	while (s[i])
	{
		if (ft_putchar_fd_int(s[i], 1) == -1)
			return (-1);
		i++;
	}
	return (result);
}

int	print_c(char c)
{
	return (ft_putchar_fd_int(c, 1));
}

int	print_u(unsigned int d)
{
	char	*str;
	int		len;

	str = ft_utoa(d);
	if (str == NULL)
		return (-1);
	len = ft_strlen(str);
	free(str);
	return (len);
}

int	print_p_x(void *voidstr, const char c)
{
	int				len;
	uintptr_t		iptr;
	unsigned int	uint;

	len = 0;
	iptr = (uintptr_t)voidstr;
	uint = (unsigned int)iptr;
	if (c == 'x')
		len = ft_putnbr_base_unsigned(uint, "0123456789abcdef");
	else if (c == 'X')
		len = ft_putnbr_base_unsigned(uint, "0123456789ABCDEF");
	else if (c == 'p')
	{
		if (ft_putchar_fd_int('0', 1) == -1)
			return (-1);
		if (ft_putchar_fd_int('x', 1) == -1)
			return (-1);
		len = ft_putnbr_base_unsigned(iptr, "0123456789abcdef");
		if (len != -1)
			len += 2;
	}
	return (len);
}
