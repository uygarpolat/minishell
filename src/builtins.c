/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hpirkola <hpirkola@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 14:02:47 by hpirkola          #+#    #+#             */
/*   Updated: 2025/01/13 16:43:53 by hpirkola         ###   ########.fr       */
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
		if (chdir(get_var(*envp, "HOME=")) != 0)
			return (print_and_return("minishell: cd: HOME not set\n"));
		return (1);
	}
	if (cmd[2])
		return (print_and_return("minishell: cd: too many arguments\n"));
	if (chdir(cmd[1]) != 0)
	{
		if (!ft_strncmp(cmd[1], "", 2))
			return (1);
		return (print_and_return("minishell: cd: No such file or directory\n"));
	}
	if (getcwd(minishell->pwd, sizeof(minishell->pwd)) == NULL)
		return (print_and_return("getcwd error\n"));
	pwd = ft_strjoin("PWD=", minishell->pwd);
	*envp = ch_var(*envp, pwd);
	free(pwd);
	if (!envp)
		return (0);
	return (1);
}

int	run_exit(t_ast *s, t_minishell *minishell, t_put *file, char ***envp)
{
	long long	i;

	if (!s->words[1])
		return (1);
	printf("exit\n");
	if (s->words[2])
	{
		ft_putstr_fd("minishell: exit: too many arguments\n", 2);
		if (!ft_isdigit(*s->words[1]) && ft_isdigit(*s->words[2]))
		{
			free_2d_array((void ***)envp);
			free_ast(&minishell->ast);
			free(minishell->p.pids);
			close_and_free(&minishell->p, file);
			exit(2);
		}
		return (0);
	}
	i = ft_atol(s->words[1]);
	if (i == 0 && ft_strncmp(s->words[1], "0", 2) && \
		ft_strncmp(s->words[1], "+0", 3) && ft_strncmp(s->words[1], "-0", 3))
	{
		ft_putstr_fd("minishell: exit: numeric argument required\n", 2);
		error(minishell, file, envp);
		free_ast(&minishell->ast);
		free_2d_array((void ***)envp);
		exit(2);
	}
	else
	{
		free_2d_array((void ***)envp);
		free_ast(&minishell->ast);
		free(minishell->p.pids);
		free_tokens(minishell->tokens, &minishell->capacity);
		close_and_free(&minishell->p, file);
		exit(i);
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
