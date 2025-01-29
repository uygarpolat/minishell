/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_utils_2.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/24 13:20:01 by upolat            #+#    #+#             */
/*   Updated: 2025/01/29 14:54:23 by hpirkola         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/ast.h"

void	pipe_fail(t_pipes *p)
{
	ft_putstr_fd("Pipe failed\n", 2);
	if (p->count > 1)
		pipe_close(p, 1, 0);
}

int	ft_pipe(t_pipes *p, int n, t_minishell *minishell, t_put *cmd)
{
	if (n % 2 == 0 && n < p->count && p->count > 0)
	{
		if (n > 0)
			pipe_close(p, 0, 0);
		if (pipe(p->pipes[0]) < 0)
			return (error(minishell, cmd), 0);
		if (n > 0)
			close(p->pipes[1][1]);
	}
	else if (n < p->count && p->count > 0)
	{
		if (n > 1)
			pipe_close(p, 1, 0);
		if (pipe(p->pipes[1]) < 0)
		{
			error(minishell, cmd);
			return (pipe_close(p, 0, 1), 0);
		}
		close(p->pipes[0][1]);
	}
	return (1);
}

char	**null_termination(char **new_envp, int flag, int j)
{
	if (flag)
		new_envp[j] = NULL;
	else
	{
		free(new_envp[j - 1]);
		new_envp[j - 1] = NULL;
	}
	return (new_envp);
}

void	assign_out_fd(t_pipes *p, int n, int *out)
{
	if (n % 2 == 0)
		*out = p->pipes[0][1];
	else
		*out = p->pipes[1][1];
}

int	failed_getcwd(t_minishell *minishell, char *old)
{
	char	*old2;
	char	*pwd;

	old2 = ft_strjoin(old, "/..");
	free(old);
	if (!old2)
		return (ft_putstr_fd("malloc fail\n", 2), 0);
	ft_strlcpy(minishell->pwd, old2, PATH_MAX);
	pwd = ft_strjoin("PWD=", minishell->pwd);
	free(old2);
	if (!pwd)
		return (ft_putstr_fd("malloc fail\n", 2), 0);
	*minishell->envp = ch_var(*minishell->envp, pwd);
	free(pwd);
	if (!minishell->envp)
		return (ft_putstr_fd("malloc fail\n", 2), 0);
	return (ft_putstr_fd("getcwd error\n", 2), 1);
}
