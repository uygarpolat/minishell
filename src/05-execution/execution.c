/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 14:14:33 by hpirkola          #+#    #+#             */
/*   Updated: 2025/01/30 19:01:06 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/ast.h"
#include "../../includes/signals.h"

static void	execute(t_ast *s, t_minishell *minishell, int n, t_put *cmd)
{
	char	*path;

	if (!ft_pipe(&minishell->p, n, minishell, cmd))
		exit(0);
	minishell->p.pids[n] = fork();
	if (minishell->p.pids[n] != 0)
		return ;
	set_signals(s->code_parser, SIGNAL_CHILD);
	if (!get_in_out(s, minishell, n, 1))
		exit (1);
	if (minishell->p.pipes || cmd->infile || cmd->outfile)
		dupping(minishell, &minishell->p, cmd, n);
	if (is_builtin(s->words))
		run_builtin(s, minishell, n, cmd);
	if (minishell->p.count > 0)
		close_pipes(minishell, n);
	no_words(s, minishell, cmd);
	path = get_path(s->words, *minishell->envp);
	error_check(path, s, minishell, cmd);
	if (g_signal == SIGINT || g_signal == SIGQUIT)
		exit(128 + g_signal);
	execve(path, s->words, *minishell->envp);
	error(minishell, cmd);
	print_and_exit(s->words[0], strerror(errno), errno, minishell);
}

static void	execute_tree(t_minishell *minishell, t_put *cmd)
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

static int	interrupted(t_minishell *minishell, t_put *cmd)
{
	unlink_here(cmd);
	exit_heredocs(cmd);
	if (minishell->p.count > 0)
		free_pipes(&minishell->p);
	free(minishell->p.pids);
	return (128 + g_signal);
}

int	execution(t_ast *s, t_token_info *token_info, t_minishell *minishell)
{
	int			i;
	t_put		cmd;

	if (!initialize(&cmd, minishell, s, token_info) || \
			!mallocing(&minishell->p))
		return (error2(minishell, "malloc failed\n", &cmd), 1);
	check_here(minishell, &cmd);
	if (g_signal == SIGINT || g_signal == SIGQUIT)
		return (interrupted(minishell, &cmd));
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
	if (minishell->here == 1)
		unlink_here(&cmd);
	free_heredocs(&cmd);
	return (s->code);
}
