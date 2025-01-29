/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_export.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 14:54:44 by hpirkola          #+#    #+#             */
/*   Updated: 2025/01/29 16:39:58 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/ast.h"

int	get_envp_size(char **envp)
{
	int	i;

	i = 0;
	while (envp[i])
		i++;
	return (i);
}

void	print_env(char **envp)
{
	int	i;

	i = -1;
	while (envp && envp[++i])
	{
		if (ft_strchr(envp[i], '='))
			printf("%s\n", envp[i]);
	}
}

static int	print_export(char **envp)
{
	int	i;
	int	j;

	i = -1;
	while (envp && envp[++i])
	{
		ft_putstr_fd("declare -x ", 1);
		j = -1;
		while (envp[i][++j])
		{
			if (envp[i][j] == '=')
			{
				ft_putchar_fd(envp[i][j], 1);
				ft_putchar_fd('"', 1);
				while (envp[i][++j])
					ft_putchar_fd(envp[i][j], 1);
				ft_putchar_fd('"', 1);
				break ;
			}
			ft_putchar_fd(envp[i][j], 1);
		}
		ft_putstr_fd("\n", 1);
	}
	return (1);
}

static void	env_not_valid_identifier(char *str, char ***str2)
{
	ft_putstr_fd("minishell: export: ", 2);
	ft_putstr_fd(str, 2);
	ft_putstr_fd(": ", 2);
	ft_putstr_fd("not a valid identifier\n", 2);
	if (str2 != NULL)
		free_2d_array((void ***)str2);
}

int	run_export(char **cmd, t_minishell *minishell)
{
	int		i;
	int		j;
	char	**str;

	if (!cmd[1])
		return (print_export(*minishell->envp));
	if (cmd[1][0] != '_' && !ft_isalpha(cmd[1][0]))
		return (env_not_valid_identifier(cmd[1], NULL), 0);
	j = 0;
	while (cmd[++j])
	{
		str = ft_split(cmd[j], '=');
		if (!str)
			return (env_not_valid_identifier(cmd[j], NULL), 0);
		i = -1;
		while (str[0][++i])
			if (str[0][i] != '_' && !ft_isalpha(str[0][i]) && \
					!ft_isdigit(str[0][i]))
				return (env_not_valid_identifier(str[0], &str), 0);
		free_2d_array((void ***)&str);
		*minishell->envp = add_env(*minishell->envp, cmd[j]);
		if (!minishell->envp)
			return (0);
	}
	return (1);
}
