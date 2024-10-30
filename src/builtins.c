/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hpirkola <hpirkola@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 14:02:47 by hpirkola          #+#    #+#             */
/*   Updated: 2024/10/30 15:35:37 by hpirkola         ###   ########.fr       */
/*   Updated: 2024/10/29 10:29:40 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ast.h"

char	**ch_envp(char **envp, char *new_dir)
{
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
	return (new_envp);
}

void	print_env(char **envp)
{
	int	i;

	i = -1;
	while (envp && envp[++i])
		printf("%s\n", envp[i]);
}

char	**rm_envp(char **envp, char *str)
{
	char	**new_envp;
	int		i;

	i = 0;
	while (envp[i])
		i++;
	new_envp = malloc(sizeof(char *) * i);
	if (new_envp == NULL)
		return (NULL);
	i = -1;
	while (envp[++i])
	{
		if (!ft_strncmp(str, envp[i], ft_strlen(str)))
			continue ;
		new_envp[i] = ft_strdup(envp[i]);
		if (!new_envp[i])
		{
			free_2d_array((void ***)&new_envp);
			return (NULL);
		}
	}
	new_envp[i] = NULL;
	return (new_envp);
}

char	**add_env(char **envp, char *str)
{
	char	**new_envp;
	int		i;


	printf("helou\n");
	i = 0;
	while (envp[i])
		i++;
	new_envp = malloc(sizeof(char *) * (i + 2));
	if (new_envp == NULL)
		return (NULL);
	i = -1;
	while (envp[++i])
	{
		new_envp[i] = ft_strdup(envp[i]);
		if (!new_envp[i])
		{
			free_2d_array((void ***)&new_envp);
			return (NULL);
		}
	}
	new_envp[i] = ft_strdup(str);
	new_envp[++i] = NULL;
	return (new_envp);
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
		if (chdir(cmd[1]) != 0)
		{
			perror("chdir error\n");
			return (0);
		}
		envp = ch_envp(envp, cmd[1]);
		if (!envp)
			return (0);
		minishell->pwd = getcwd(buf, sizeof(buf));
		if (!minishell->pwd)
		{
			perror("getcwd error\n");
			return (0);
		}
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
	{
		envp = add_env(envp, cmd[1]);
		if (!envp)
		{
			perror("export error\n");
			return (0);
		}
	}
	else if (!ft_strncmp(cmd[0], "unset", 6))
	{
		envp = rm_envp(envp, cmd[1]);
		if (!envp)
		{
			perror("unset error\n");
			return (0);
		}
	}
	else if (!ft_strncmp(cmd[0], "env", 4))
		print_env(envp);
	return (1);
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
