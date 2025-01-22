/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atol.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/25 17:31:26 by hpirkola          #+#    #+#             */
/*   Updated: 2025/01/22 19:41:38 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/ast.h"

static long long	to_int(const char *str, int neg)
{
	int			i;
	long long	result;

	i = 0;
	result = 0;
	while (str[i] >= '0' && str[i] <= '9')
	{
		if ((result > 9223372036854775807 / 10 && neg == 1) \
			|| (result == 9223372036854775807
				/ 10 && str[i] - '0' > 7 && neg == 1))
			return (0);
		else if (result == 9223372036854775807
			/ 10 && str[i] - '0' > 8 && neg == -1)
			return (0);
		result = result * 10 + (str[i] - '0');
		i++;
	}
	return (result * (long long) neg);
}

long long	ft_atol(const char *str)
{
	int			neg;
	long long	result;

	neg = 1;
	result = 0;
	while (*str == ' ' || *str == '\n' || *str == '\t' || *str == '\v' \
			|| *str == '\f' || *str == '\r')
		str++;
	if (*str == '-' || *str == '+')
	{
		if (*str == '-')
			neg *= -1;
		str++;
	}
	result = to_int(str, neg);
	return (result);
}
