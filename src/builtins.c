/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hpirkola <hpirkola@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 14:02:47 by hpirkola          #+#    #+#             */
/*   Updated: 2024/11/11 14:52:42 by hpirkola         ###   ########.fr       */
/*   Updated: 2024/10/29 10:29:40 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ast.h"

char	**ch_envp(char **envp, char *new_dir)
{
	char	*dir;
	char	**new_envp;
	char	**tmp;

	tmp = envp;
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
	dir = ft_strjoin_free(dir, new_dir);
	if (!dir)
		return (NULL);
	new_envp = ft_strdup3(tmp, dir);
	if (!new_envp)
		return (NULL);
	if (envp)
		free_2d_array((void ***)&tmp);
	free(dir);
	return (new_envp);
}

char	**rm_envp(char **envp, char *str)
{
	char	**new_envp;
	int		i;
	int		flag;

	i = 0;
	flag = 0;
	while (envp[i])
		i++;
	new_envp = ft_calloc((size_t) i, sizeof(char *));
	if (new_envp == NULL)
		return (NULL);
	i = -1;
	while (envp[++i])
	{
		if (!ft_strncmp(str, envp[i], ft_strlen(str)))
		{
			flag = 1;
			continue ;
		}
		new_envp[i] = ft_strdup(envp[i]);
		if (!new_envp[i])
		{
			free_2d_array((void ***)&new_envp);
			return (NULL);
		}
	}
	if (flag)
	{
		new_envp[i - 1] = NULL;
		free_2d_array((void ***)&envp);
		return (new_envp);
	}
	free_2d_array((void ***)&new_envp);
	return (envp);
}

char	**add_env(char **envp, char *str)
{
	char	**new_envp;
	int		i;


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
	free_2d_array((void ***)&envp);
	return (new_envp);
}

void	print_env(char **envp)
{
	int	i;

	i = -1;
	while (envp && envp[++i])
		printf("%s\n", envp[i]);
}

int	execute_builtin(char **cmd, char ***envp, t_minishell *minishell)
{
	int	i;
	char	**str;

	if (!ft_strncmp(cmd[0], "echo", 5))
	{
		if (cmd[2] && !ft_strncmp(cmd[1], "-n", 3))
		{
			i = 1;
			while (cmd[++i])
			{
				if (i > 2)
					printf(" ");
				printf("%s", cmd[i]);
			}
		}
		else
		{
			i = 0;
			while (cmd[++i])
			{
				if (i > 1)
					printf(" ");
				printf("%s", cmd[i]);
			}
			printf("\n");
		}
	}
	else if (!ft_strncmp(cmd[0], "cd", 3))
	{
		if (chdir(cmd[1]) != 0)
			return (0);
		*envp = ch_envp(*envp, cmd[1]);
		if (!envp)
			return (0);
		if (getcwd(minishell->pwd, sizeof(minishell->pwd) != 0))
		{
			perror("getcwd error\n");
			return (0);
		}
	}
	else if (!ft_strncmp(cmd[0], "pwd", 4))
		printf("%s\n", minishell->pwd);
	else if (!ft_strncmp(cmd[0], "export", 7))
	{
		if (cmd[1][0] != '_' && !ft_isalpha(cmd[1][0]))
		{
			ft_putstr_fd("not a valid identifier\n", 2);
			return (0);
		}
		str = ft_split(cmd[1], '=');
		if (!str)
		{
			ft_putstr_fd("not a valid identifier\n", 2);
			return (0);
		}
		i = -1;
		while (str[0][++i])
		{
			if (str[0][i] != '_' && !ft_isalpha(str[0][i]) && !ft_isdigit(str[0][i]))
			{
				ft_putstr_fd("not a valid identifier\n", 2);
				return (0);
			}
		}
		*envp = add_env(*envp, cmd[1]);
		if (!envp)
			return (0);
	}
	else if (!ft_strncmp(cmd[0], "unset", 6))
	{
		*envp = rm_envp(*envp, cmd[1]);
		if (!envp)
			return (0);
	}
	else if (!ft_strncmp(cmd[0], "env", 4))
		print_env(*envp);
	else if (!ft_strncmp(cmd[0], "exit", 5))
	{
		i = ft_atoi(cmd[1]);
		if (i == 0 && ft_strncmp(cmd[1], "0", 2) && ft_strncmp(cmd[1], "+0", 3) && ft_strncmp(cmd[1], "-0", 3))
			exit(1);
		else
			exit(i);
	}
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
	else if (!ft_strncmp(cmd[0], "exit", 5))
		return (1);
	return (0);
}
