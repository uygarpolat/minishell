/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 17:16:21 by upolat            #+#    #+#             */
/*   Updated: 2025/01/23 19:08:11 by hpirkola         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/ast.h"

void	no_words(t_ast *s, t_minishell *minishell, t_put *cmd)
{
	if (!*s->words)
	{
		error(minishell, cmd);
		if (minishell->p.count == 0)
		{
			free_heredocs(cmd);
			free(cmd->cmd_fd);
		}
		free_ast(&minishell->ast);
		exit(0);
	}
}

void	pipe_close(t_pipes *p, int pipe_no, int flag)
{
	if (flag)
		ft_putstr_fd("Pipe failed\n", 2);
	close(p->pipes[pipe_no][0]);
	close(p->pipes[pipe_no][1]);
}

void	set_pwd(t_minishell *minishell, char **envp)
{
	if (getcwd(minishell->pwd, PATH_MAX) == NULL)
	{
		if (!get_var(envp, "PWD="))
			ft_strlcpy(minishell->pwd, "null", PATH_MAX);
		else
			ft_strlcpy(minishell->pwd, get_var(envp, "PWD="), PATH_MAX);
	}
}

int	check_newlines(char **cmd)
{
	int	i;
	int	n;
	int	flag;

	i = 0;
	while (cmd[++i] && cmd[i][0] == '-')
	{
		flag = 1;
		n = 0;
		while (cmd[i][++n])
		{
			if (cmd[i][n] != 'n')
			{
				flag = 0;
				break ;
			}
		}
		if (!flag)
			break ;
	}
	return (i);
}
