/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atol.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hpirkola <hpirkola@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/25 17:31:26 by hpirkola          #+#    #+#             */
/*   Updated: 2024/12/04 15:02:45 by hpirkola         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ast.h"

static unsigned long long	to_int(const char *str, int neg)
{
	int					i;
	unsigned long long	result;

	i = 0;
	result = 0;
	while (str[i] >= '0' && str[i] <= '9')
	{
		if ((result > 9223372036854775807 / 10 && neg == 1) \
			|| (result == 9223372036854775807 / 10 && str[i] - '0' > 7))
			return (0);
		result = result * 10 + (str[i] - '0');
		i++;
	}
	return (result * neg);
}

int	ft_atol(const char *str)
{
	int	neg;
	int	result;

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
	result = (int) to_int(str, neg);
	return (result);
}
