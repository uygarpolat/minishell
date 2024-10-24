/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hpirkola <hpirkola@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 14:02:47 by hpirkola          #+#    #+#             */
/*   Updated: 2024/10/24 15:07:28 by hpirkola         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ast.h"

int	execute_builtin(char **cmd)
{
	char	*buf;
	int	i;

	buf = NULL;
	if (!ft_strncmp(cmd[0], "echo", 4))
	{
		i = 0;
		while (cmd[++i])
			printf("%s ", cmd[i]);
		printf("\n");
	}
	else if (!ft_strncmp(cmd[0], "cd", 2))
		chdir(cmd[1]);
	else if (!ft_strncmp(cmd[0], "pwd", 3))
	{
		buf = malloc(sizeof(char) * ft_strlen(cmd[1]) + 1);
		if (!buf)
			return (0);
		getcwd(buf, ft_strlen(cmd[1]));
	}
	else if (!ft_strncmp(cmd[0], "export", 6))
		return (1);
	else if (!ft_strncmp(cmd[0], "unset", 5))
		return (1);
	else if (!ft_strncmp(cmd[0], "env", 3))
		return (1);
	return (0);
}

int	is_builtin(t_tokens *token)
{
	if (!ft_strncmp(token->value, "echo", 4))
		return (1);
	else if (!ft_strncmp(token->value, "cd", 2))
		return (1);
	else if (!ft_strncmp(token->value, "pwd", 3))
		return (1);
	else if (!ft_strncmp(token->value, "export", 6))
		return (1);
	else if (!ft_strncmp(token->value, "unset", 5))
		return (1);
	else if (!ft_strncmp(token->value, "env", 3))
		return (1);
	return (0);
}
