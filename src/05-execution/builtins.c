/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hpirkola <hpirkola@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 14:02:47 by hpirkola          #+#    #+#             */
/*   Updated: 2025/01/30 15:58:35 by hpirkola         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/ast.h"

int	run_echo(char **cmd)
{
	int	i;
	int	n;

	if (!cmd[1])
	{
		ft_putstr_fd("\n", 1);
		return (1);
	}
	i = check_newlines(cmd);
	n = i;
	while (cmd[i])
	{
		if (i > n)
			ft_putstr_fd(" ", 1);
		ft_putstr_fd(cmd[i], 1);
		i++;
	}
	if (n == 1)
		ft_putstr_fd("\n", 1);
	return (1);
}

static int	get_pwd(t_minishell *minishell)
{
	char	*pwd;
	char	*old_pwd;
	char	*old;

	old_pwd = ft_strjoin("OLDPWD=", minishell->pwd);
	if (!old_pwd)
		return (ft_putstr_fd("malloc fail\n", 2), 0);
	*minishell->envp = ch_var(*minishell->envp, old_pwd);
	free(old_pwd);
	if (!minishell->envp)
		return (ft_putstr_fd("malloc fail\n", 2), 0);
	old = ft_strdup(minishell->pwd);
	if (!old)
		return (ft_putstr_fd("malloc fail\n", 2), 0);
	if (getcwd(minishell->pwd, sizeof(minishell->pwd)) == NULL)
		return (failed_getcwd(minishell, old));
	free(old);
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
	if (!cmd[1] || !ft_strncmp(cmd[1], "--", 3) || !ft_strncmp(cmd[1], "~", 2))
	{
		if (!get_var(*minishell->envp, "HOME=")
			|| chdir(get_var(*minishell->envp, "HOME=")) != 0)
			return (ft_putstr_fd("minishell: cd: HOME not set\n", 2), 0);
		if (!get_pwd(minishell))
			return (ft_putstr_fd("couldn't retrieve" \
				"current working directory\n", 2), 0);
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
	close_and_free(&minishell->p, file, 0);
	exit(i);
}

int	run_exit(t_ast *s, t_minishell *minishell, t_put *file)
{
	long long	i;

	if (!s->words[1])
		return (1);
	ft_putstr_fd("exit\n", 1);
	if (s->words[2])
		return (too_many_args(s, minishell, file), 0);
	i = ft_atol(s->words[1]);
	if ((i == 0 && ft_strncmp(s->words[1], "0", 2) && \
		ft_strncmp(s->words[1], "+0", 3) && ft_strncmp(s->words[1], "-0", 3)) \
			|| !is_numeric(s, 1))
	{
		ft_putstr_fd("minishell: exit: numeric argument required\n", 2);
		error(minishell, file);
		free_ast(&minishell->ast);
		exit(2);
	}
	else
		graceful_exit(minishell, file, i, 1);
	return (1);
}
