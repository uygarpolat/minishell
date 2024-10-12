/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/03 08:47:38 by upolat            #+#    #+#             */
/*   Updated: 2024/05/23 22:42:55 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	caster(va_list *arg, const char c)
{
	int	result;

	result = 0;
	if (c == 'd' || c == 'i')
		result = print_d_i(va_arg(*arg, int));
	if (c == 's')
		result = print_s(va_arg(*arg, char *));
	if (c == 'c')
		result = print_c(va_arg(*arg, int));
	if (c == 'p' || c == 'x' || c == 'X')
		result = print_p_x(va_arg(*arg, void *), c);
	if (c == 'u')
		result = print_u(va_arg(*arg, unsigned int));
	if (result == -1)
		return (-1);
	return (result);
}

int	ft_printf(const char *s, ...)
{
	va_list	arg;
	int		res;
	int		i;
	int		total_result;

	i = 0;
	total_result = 0;
	va_start(arg, s);
	while (s[i])
	{
		if (s[i] == '%')
		{
			if (s[++i] == '%')
				res = print_c(s[i++]);
			else
				res = caster(&arg, s[i++]);
		}
		else
			res = ft_putchar_fd_int(s[i++], 1);
		if (res == -1)
			return (-1);
		total_result = total_result + res;
	}
	va_end(arg);
	return (total_result);
}
