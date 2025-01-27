/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 14:14:33 by hpirkola          #+#    #+#             */
/*   Updated: 2025/01/27 11:27:58 by hpirkola         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/ast.h"
#include "../../includes/signals.h"

void	execute(t_ast *s, t_minishell *minishell, int n, t_put *cmd)
{
	char	*path;

	if (!ft_pipe(&minishell->p, n, minishell, cmd))
		exit(0);
	minishell->p.pids[n] = fork();
	if (minishell->p.pids[n] != 0)
		return ;
	set_signals(s->code_parser, SIGNAL_CHILD);
	get_in_out(s, cmd, minishell, n);
	if (minishell->p.pipes || cmd->infile || cmd->outfile)
		dupping(minishell, &minishell->p, cmd, n);
	if (is_builtin(s->words))
		run_builtin(s, minishell, n, cmd);
	if (minishell->p.count > 0)
		close_pipes(minishell, n);
	no_words(s, minishell, cmd);
	path = get_path(s->words, *minishell->envp);
	error_check(path, s, minishell, cmd);
	if (g_signal == 130)
		exit(130);
	execve(path, s->words, *minishell->envp);
	error(minishell, cmd);
	print_and_exit(s->words[0], strerror(errno), errno, minishell);
}

void	execute_tree(t_minishell *minishell, t_put *cmd)
{
	int		n;
	t_ast	*ast;

	ast = minishell->ast;
	n = 0;
	while (ast)
	{
		if (ast->type == AST_PIPE)
			execute(ast->left, minishell, n, cmd);
		else if (ast->type == AST_COMMAND)
			execute(ast, minishell, n, cmd);
		n++;
		if (ast)
			ast = ast->right;
	}
}

int	init_heredocs(t_put *cmd)
{
	int		i;
	char	*index;

	i = -1;
	while (++i < 16)
	{
		index = ft_itoa(i);
		if (!index)
			return (0);
		cmd->heredocs[i] = ft_strjoin(".heredoc", index);
		free(index);
		if (!cmd->heredocs[i])
			return (0);
	}
	return (1);
}

int	initialize(t_put *cmd, t_minishell *minishell,
		t_ast *s, t_token_info *token_info)
{
	int	i;

	cmd->infile = NULL;
	cmd->outfile = NULL;
	cmd->in = -1;
	cmd->out = -1;
	cmd->stdin2 = -1;
	cmd->stdout2 = -1;
	i = -1;
	while (++i < 16)
		cmd->fd_here[i] = -1;
	minishell->ast = s;
	minishell->p.count = count_pipes(minishell->ast);
	minishell->tokens = token_info->tokens;
	minishell->capacity = token_info->capacity;
	minishell->envp = token_info->envp;
	s->code = 0;
	if (!init_heredocs(cmd))
		return (0);
	cmd->cmd_fd = malloc(sizeof(int) * (minishell->p.count + 1));
	if (!cmd->cmd_fd)
		return (0);
	return (1);
}

void	free_pipes(t_pipes *p)
{
	int	i;
	
	i = -1;
	while (++i < p->count)
		free(p->pipes[i]);
	free(p->pipes);
}

int	execution(t_ast *s, t_token_info *token_info, t_minishell *minishell)
{
	int			i;
	t_put		cmd;

	if (!initialize(&cmd, minishell, s, token_info) || \
			!mallocing(&minishell->p))
		return (error2(minishell, "malloc failed\n", &cmd), 1);
	check_here(minishell, &cmd);
	if (g_signal == 130)
	{
		free_heredocs(&cmd);
		free(cmd.cmd_fd);
		if (minishell->p.count > 0)
			free_pipes(&minishell->p);
		return (free(minishell->p.pids), 130);
	}
	if (minishell->p.count == 0 && is_builtin(s->words))
		return (only_builtin(minishell, &cmd));
	else
		execute_tree(minishell, &cmd);
	close_and_free(&minishell->p, &cmd, 1);
	i = 0;
	while (i <= minishell->p.count)
		s->code = waiting(minishell->p.pids[i++]);
	if (minishell->p.pids)
		free(minishell->p.pids);
	unlink_here(&cmd);
	free_heredocs(&cmd);
	return (s->code);
}
