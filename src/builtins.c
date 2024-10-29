/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hpirkola <hpirkola@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 14:02:47 by hpirkola          #+#    #+#             */
/*   Updated: 2024/10/29 10:29:40 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ast.h"

char	**ch_envp(char **envp, char *new_dir)
{
	//int	i;
	char	*dir;
	char	**new_envp;
	//char	**all_dir;

	while (*envp)
	{
		if (!ft_strncmp(*envp, "PWD=", 4))
		{
			dir = *envp + 4;
			break ;
		}
		envp++;
	}
	//all_dir = ft_split(dir, '/');
	dir = ft_strjoin(dir, "/");
	dir = ft_strjoin(dir, new_dir);
	new_envp = ft_strdup3(envp, dir);
	//i = -1;
	//while (envp[++i])
		//free(envp[i]);
	//free (envp);
	return (new_envp);
	//return (0) //no access
}

int	execute_builtin(char **cmd, char **envp, t_minishell *minishell)
{
	char	buf[PATH_MAX];
	int	i;

	if (!ft_strncmp(cmd[0], "echo", 5))
	{
		i = 0;
		while (cmd[++i])
		{
			if (i == 1 && !ft_strncmp(cmd[i], "-n", 3))
				continue ;
			printf("%s", cmd[i]);
			if (cmd[i + 1])
				printf(" ");
		}
		if (ft_strncmp(cmd[1], "-n", 3))
			printf("\n");
	}
	else if (!ft_strncmp(cmd[0], "cd", 3))
	{
		//ch_envp(envp, cmd[1]);
		if (chdir(cmd[1]) != 0)
			perror("chdir error\n");
		envp = ch_envp(envp, cmd[1]);
		minishell->pwd = getcwd(buf, sizeof(buf));
		if (!minishell->pwd)
			perror("getcwd error\n");
	}
	else if (!ft_strncmp(cmd[0], "pwd", 4))
	{
		printf("%s\n", minishell->pwd);
		//if (getcwd(buf, sizeof(buf)))
			//printf("%s\n", buf);
		//else
			//perror("getcwd error\n");
	}
	else if (!ft_strncmp(cmd[0], "export", 7))
		return (1);
	else if (!ft_strncmp(cmd[0], "unset", 6))
		return (1);
	else if (!ft_strncmp(cmd[0], "env", 4))
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
