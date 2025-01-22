/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 14:02:47 by hpirkola          #+#    #+#             */
/*   Updated: 2025/01/22 17:23:27 by upolat           ###   ########.fr       */
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
	i = check_newlines(cmd);
	n = i;
	while (cmd[i])
	{
		if (i > n)
			printf(" ");
		printf("%s", cmd[i]);
		i++;
	}
	if (n == 1)
		printf("\n");
	return (1);
}

int	get_pwd(t_minishell *minishell)
{
	char	*pwd;

	pwd = ft_strjoin("PWD=", minishell->pwd);
	if (!pwd)
		return (ft_putstr_fd("malloc fail\n", 2), 0);
	*minishell->envp = ch_var(*minishell->envp, pwd);
	free(pwd);
	if (!minishell->envp)
		return (ft_putstr_fd("malloc fail\n", 2), 0);
	return (1);
}

int	run_cd(char **cmd, t_minishell *minishell)
{
	if (!cmd[1])
	{
		if (!get_var(*minishell->envp, "HOME=")
			|| chdir(get_var(*minishell->envp, "HOME=")) != 0)
			return (ft_putstr_fd("minishell: cd: HOME not set\n", 2), 0);
		return (1);
	}
	if (cmd[2])
		return (ft_putstr_fd("minishell: cd: too many arguments\n", 2), 0);
	if (chdir(cmd[1]) != 0)
	{
		if (!ft_strncmp(cmd[1], "", 2))
			return (1);
		return (print_and_return("minishell: cd: ", cmd[1],
				": No such file or directory\n"));
	}
	if (getcwd(minishell->pwd, sizeof(minishell->pwd)) == NULL)
		return (ft_putstr_fd("getcwd error\n", 2), 0);
	if (!get_pwd(minishell))
		return (0);
	return (1);
}

void	graceful_exit(t_minishell *minishell, t_put *file,
			long long i, int flag)
{
	free_2d_array((void ***)minishell->envp);
	free_ast(&minishell->ast);
	free(minishell->p.pids);
	if (flag)
		free_tokens(&minishell->tokens, &minishell->capacity);
	close_and_free(&minishell->p, file);
	exit(i);
}

int	run_exit(t_ast *s, t_minishell *minishell, t_put *file)
{
	long long	i;

	if (!s->words[1])
		return (1);
	printf("exit\n");
	if (s->words[2])
	{
		ft_putstr_fd("minishell: exit: too many arguments\n", 2);
		if (!ft_isdigit(*s->words[1]) && ft_isdigit(*s->words[2]))
			graceful_exit(minishell, file, 2, 0);
		return (0);
	}
	i = ft_atol(s->words[1]);
	if (i == 0 && ft_strncmp(s->words[1], "0", 2) && \
		ft_strncmp(s->words[1], "+0", 3) && ft_strncmp(s->words[1], "-0", 3))
	{
		ft_putstr_fd("minishell: exit: numeric argument required\n", 2);
		error(minishell, file);
		free_ast(&minishell->ast);
		//free_2d_array((void ***)envp);
		exit(2);
	}
	else
		graceful_exit(minishell, file, i, 1);
	return (1);
}
