/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/12 01:01:52 by upolat            #+#    #+#             */
/*   Updated: 2024/04/23 18:22:09 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_digit_counter(int n)
{
	int		digit;

	if (n == -2147483648)
		return (11);
	if (n < 0)
	{
		digit = 2;
		n = -n;
	}
	else
		digit = 1;
	while (n > 9)
	{
		digit++;
		n = n / 10;
	}
	return (digit);
}

static void	ft_strcharcat(char *str, char c)
{
	int	i;

	i = 0;
	while (str[i] != '\0')
		i++;
	str[i] = c;
	str[i + 1] = '\0';
}

static void	ft_putnbr(char *str, int n)
{
	char	c;

	if (n == -2147483648)
	{
		ft_strcharcat(str, '-');
		ft_strcharcat(str, '2');
		n = 147483648;
	}
	else if (n < 0)
	{
		n = -n;
		ft_strcharcat(str, '-');
	}
	if (n >= 10)
	{
		ft_putnbr(str, n / 10);
		ft_putnbr(str, n % 10);
	}
	else
	{
		c = n + '0';
		ft_strcharcat(str, c);
	}
}

char	*ft_itoa(int n)
{
	int		digit_count;
	char	*str;

	digit_count = ft_digit_counter(n);
	str = malloc(sizeof(char) * (digit_count + 1));
	if (str == NULL)
		return (NULL);
	str[0] = '\0';
	ft_putnbr(str, n);
	return (str);
}
