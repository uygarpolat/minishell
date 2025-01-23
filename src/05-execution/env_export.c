/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_export.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 14:54:44 by hpirkola          #+#    #+#             */
/*   Updated: 2025/01/23 12:59:39 by hpirkola         ###   ########.fr       */
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

int	print_export(char **envp)
{
	int	i;

	i = -1;
	while (envp && envp[++i])
	{
		printf("declare -x ");
		printf("%s\n", envp[i]);
	}
	return (1);
}

void	env_not_valid_identifier(char *str, char ***str2)
{
	ft_putstr_fd("minishell: export: ", 2);
	ft_putstr_fd(str, 2);
	ft_putstr_fd(": ", 2);
	ft_putstr_fd("not a valid identifier\n", 2);
	if (str2 != NULL)
		free_2d_array((void ***)str2);
}

int	run_export(char **cmd, char ***envp)
{
	int		i;
	int		j;
	char	**str;

	if (!cmd[1])
		return (print_export(*envp));
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
		*envp = add_env(*envp, cmd[j]);
		if (!envp)
			return (0);
	}
	return (1);
}
