/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/11 02:20:08 by upolat            #+#    #+#             */
/*   Updated: 2024/04/23 23:41:12 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_word_amount_checker(char const *str, int c, int flag)
{
	int	counter;

	if (!*str)
		return (0);
	counter = 0;
	if (str[0] != c)
		counter = 1;
	while (*str)
	{
		while (*str == c)
		{
			str++;
			flag = 1;
		}
		if (flag == 1)
		{
			if (!*str)
				break ;
			flag = 0;
			counter++;
			str--;
		}
		str++;
	}
	return (counter);
}

static void	copy_characters(char *s, const char *str, int i, int counter)
{
	int	flag;

	flag = 0;
	while (flag < counter)
	{
		s[flag] = str[i - counter];
		i++;
		flag++;
	}
}

static char	*ft_word_length_checker(char const *str, char c, int i, int counter)
{
	int		flag;
	char	*s;

	flag = 0;
	while (str[i] != '\0')
	{
		while (str[i] && str[i] != c)
		{
			flag = 1;
			counter++;
			i++;
		}
		if (flag == 1)
			break ;
		i++;
	}
	s = malloc(sizeof(char) * (counter + 1));
	if (s == NULL)
		return (NULL);
	flag = 0;
	copy_characters(s, str, i, counter);
	s[counter] = '\0';
	return (s);
}

static int	ft_strlen_mod(char const *str, char c, int i)
{
	int	len;
	int	flag;

	len = 0;
	flag = 1;
	while (str[i] != '\0')
	{
		if (str[i] != c)
		{
			flag = 0;
			len++;
		}
		else if (flag == 0)
			break ;
		else
			len++;
		i++;
	}
	return (len);
}

char	**ft_split(char const *s, char c)
{
	int		i;
	int		h;
	int		counter;
	char	**str;

	counter = ft_word_amount_checker(s, c, 0);
	str = malloc(sizeof(char *) * (counter + 1));
	if (str == NULL)
		return (NULL);
	i = -1;
	h = 0;
	while (i++ < counter - 1)
	{
		str[i] = ft_word_length_checker(s, c, h, 0);
		if (str[i] == NULL)
		{
			while (--i >= 0)
				free(str[i]);
			free(str);
			return (NULL);
		}
		h = h + ft_strlen_mod(s, c, h) + 1;
	}
	str[i] = 0;
	return (str);
}
