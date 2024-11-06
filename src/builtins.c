/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hpirkola <hpirkola@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 14:02:47 by hpirkola          #+#    #+#             */
/*   Updated: 2024/11/06 12:47:41 by hpirkola         ###   ########.fr       */
/*   Updated: 2024/10/29 10:29:40 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ast.h"

char	**ch_envp(char **envp, char *new_dir)
{
	//int	i;
	char	*dir;
	char	**new_envp;

	while (*envp)
	{
		if (!ft_strncmp(*envp, "PWD=", 4))
		{
			dir = *envp + 4;
			break ;
		}
		envp++;
	}
	dir = ft_strjoin(dir, "/");
	dir = ft_strjoin(dir, new_dir);
	if (!dir)
		return (NULL);
	new_envp = ft_strdup3(envp, dir);
	if (!new_envp)
		return (NULL);
	//i = -1;
	//while (envp[++i])
		//free(envp[i]);
	//free (envp);
	return (new_envp);
}

int	execute_builtin(char **cmd, char **envp, t_minishell *minishell)
{
	//char	buf[PATH_MAX];
	//int	i;
	//char	**pwd;

	if (!ft_strncmp(cmd[0], "echo", 5))
	{
		if (cmd[2] && !ft_strncmp(cmd[1], "-n", 3))
			printf("%s", cmd[2]);
		else
			printf("%s\n", cmd[1]);
	}
	else if (!ft_strncmp(cmd[0], "cd", 3))
	{
		if (chdir(cmd[1]) != 0)
		{
			perror("chdir error\n");
			return (0);
		}
		envp = ch_envp(envp, cmd[1]);
		if (!envp)
			return (0);
		if (getcwd(minishell->pwd, sizeof(minishell->pwd) != 0))
		{
			perror("getcwd error\n");
			return (0);
		}
	}
	else if (!ft_strncmp(cmd[0], "pwd", 4))
	{
		/*i = 0;
		while (envp[i])
		{
			if (!ft_strncmp("PWD=", envp[i], 4))
			{
				pwd = ft_split(envp[i], '=');
				printf("%s\n", pwd[1]);
			}
			i++;
		}*/
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
	return (1);
}

int	is_builtin(char **cmd)
{
	if (!ft_strncmp(cmd[0], "echo", 5))
		return (1);
	else if (!ft_strncmp(cmd[0], "cd", 3))
		return (1);
	else if (!ft_strncmp(cmd[0], "pwd", 4))
		return (1);
	else if (!ft_strncmp(cmd[0], "export", 7))
		return (1);
	else if (!ft_strncmp(cmd[0], "unset", 6))
		return (1);
	else if (!ft_strncmp(cmd[0], "env", 4))
		return (1);
	return (0);
}
