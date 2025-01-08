/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hpirkola <hpirkola@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 14:02:47 by hpirkola          #+#    #+#             */
/*   Updated: 2025/01/08 11:55:04 by hpirkola         ###   ########.fr       */
/*   Updated: 2024/10/29 10:29:40 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ast.h"

int	run_echo(char **cmd)
{
	int	i;
	int	n;

	if (!cmd[1])
	{
		printf("\n");
		return (1);
	}
	n = 0;
	if (!ft_strncmp(cmd[1], "-n", 3))
		n = 1;
	i = 0 + n;
	while (cmd[++i])
	{
		if (i > 1 + n)
			printf(" ");
		printf("%s", cmd[i]);
	}
	if (n == 0)
		printf("\n");
	return (1);
}

char	*get_var(char **envp, char *key)
{
	while (*envp)
	{
		if (!ft_strncmp(*envp, key, ft_strlen(key)))
			return (*envp + ft_strlen(key));
		envp++;
	}
	return (NULL);
}

int	run_cd(char ***envp, char **cmd, t_minishell *minishell)
{
	char	*pwd;

	if (!cmd[1])
	{
		if (chdir(get_var(*envp,"HOME=")) != 0)
		{
			ft_putstr_fd("minishell: cd: HOME not set\n", 2);
			return (0);
		}
		return (1);
	}
	if (cmd[2])
	{
		ft_putstr_fd("minishell: cd: too many arguments\n", 2);
		return (0);
	}
	if (chdir(cmd[1]) != 0)
	{
		if (!ft_strncmp(cmd[1], "", 2))
			return (1);
		ft_putstr_fd("minishell: cd: No such file or directory\n", 2);
		return (0);
	}
	//*envp = ch_envp(*envp, cmd[1]);
	//if (!envp)
		//return (0);
	if (getcwd(minishell->pwd, sizeof(minishell->pwd)) == NULL)
	{
		ft_putstr_fd("getcwd error\n", 2);
		return (0);
	}
	pwd = malloc(sizeof(char) * 5);
	if (!pwd)
	{
		ft_putstr_fd("malloc fail\n", 2);
		return (0);
	}
	pwd = ft_strjoin("PWD=", minishell->pwd);
	*envp = ch_var(*envp, pwd);
	if (!envp)
		return (0);
	return (1);
}

int	run_exit(char **cmd, t_minishell *minishell, t_put *file, char ***envp)
{
	long long	i;

	//if we are in a child process, we should free everything before exiting???
	if (!cmd[1])
	{
		if (minishell->p.count > 0)
			free_2d_array((void ***)envp);
		close_and_free(&minishell->p, file);
		free(minishell->p.pids);
		free_ast(&minishell->ast);
		exit(0);
	}
	printf("exit\n");
	if (cmd[2])
	{
		ft_putstr_fd("minishell: exit: too many arguments\n", 2);
		if (!ft_isdigit(*cmd[1]) && ft_isdigit(*cmd[2]))
			exit(2);
		return (0);
	}
	i = ft_atol(cmd[1]);
	if (i == 0 && ft_strncmp(cmd[1], "0", 2) && ft_strncmp(cmd[1], "+0", 3) \
		&& ft_strncmp(cmd[1], "-0", 3))
	{
		ft_putstr_fd("minishell: exit: numeric argument required\n", 2);
		error(minishell, file);
		exit(2);
	}
	else
	{
		if (minishell->p.pipes > 0)
			free_2d_array((void ***)envp);
		exit(i);
	}
	return (1);
}

int	execute_builtin(t_ast *s, char **cmd, char ***envp, t_minishell *minishell, int n, t_put *file)
{
	//remove variables by accesing cmd from s and something else too
	if (minishell->p.count == 0)
		check_in_out(s, minishell, file, n);
	if (!ft_strncmp(cmd[0], "echo", 5))
		return (run_echo(cmd));
	else if (!ft_strncmp(cmd[0], "cd", 3))
		return (run_cd(envp, cmd, minishell));
	else if (!ft_strncmp(cmd[0], "pwd", 4))
	{
		//printf("%s\n", minishell->pwd);
		//what if you have unsetted pwd?? check if get_var returns null first?
		printf("%s\n", get_var(*envp, "PWD="));
	}
	else if (!ft_strncmp(cmd[0], "export", 7))
		return (run_export(cmd, envp));
	else if (!ft_strncmp(cmd[0], "unset", 6))
	{
		if (cmd[1])
			*envp = rm_envp(*envp, cmd[1]);
		if (!envp)
			return (0);
	}
	else if (!ft_strncmp(cmd[0], "env", 4))
		print_env(*envp);
	else if (!ft_strncmp(cmd[0], "exit", 5))
	{
		if (!run_exit(cmd, minishell, file, envp))
			return (0);
	}
	return (1);
}

int	is_builtin(char **cmd)
{
	if (!cmd || !cmd[0])
		return (0);
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
