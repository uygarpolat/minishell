/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 14:14:33 by hpirkola          #+#    #+#             */
/*   Updated: 2025/01/22 21:40:27 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/ast.h"
#include "../../includes/signals.h"

int	ft_pipe(t_pipes *p, int n)
{
	if (n % 2 == 0 && n < p->count && p->count > 0)
	{
		if (n > 0)
			pipe_close(p, 0, 0);
		if (pipe(p->pipes[0]) < 0)
		{
			ft_putstr_fd("Pipe failed\n", 2);
			if (p->count > 1)
				pipe_close(p, 1, 0);
			return (0);
		}
		if (n > 0)
			close(p->pipes[1][1]);
	}
	else if (n < p->count && p->count > 0)
	{
		if (n > 1)
			pipe_close(p, 1, 0);
		if (pipe(p->pipes[1]) < 0)
			return (pipe_close(p, 0, 1), 0);
		close(p->pipes[0][1]);
	}
	return (1);
}

void	execute(t_ast *s, t_minishell *minishell, int n, t_put *cmd)
{
	char	*path;

	if (!ft_pipe(&minishell->p, n))
		return ;//pipe error
	minishell->p.pids[n] = fork();
	if (minishell->p.pids[n] != 0)
		return ;
	set_signals(s->code_parser, SIGNAL_CHILD);
	get_in_out(s, cmd, minishell);
	if (minishell->p.pipes || cmd->infile || cmd->outfile)
		dupping(minishell, &minishell->p, cmd, n);
	if (minishell->p.count > 0)
		close_pipes(minishell, n);
	if (is_builtin(s->words))
		run_builtin(s, minishell, n, cmd);
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

void	initialize(t_put *cmd, t_minishell *minishell,
		t_ast *s, t_token_info *token_info)
{
	cmd->infile = NULL;
	cmd->outfile = NULL;
	cmd->in = -1;
	cmd->out = -1;
	cmd->stdin2 = -1;
	cmd->stdout2 = -1;
	minishell->ast = s;
	minishell->p.count = count_pipes(minishell->ast);
	minishell->p.o_count = count_operators(minishell->ast);
	minishell->tokens = token_info->tokens;
	minishell->capacity = token_info->capacity;
	minishell->envp = token_info->envp;
	s->code = 0;
}

int	execution(t_ast *s, t_token_info *token_info, t_minishell *minishell)
{
	int			i;
	t_put		cmd;

	initialize(&cmd, minishell, s, token_info);
	if (!mallocing(&minishell->p))
		return (error2(minishell, "malloc failed\n", &cmd), 1);
	check_here(minishell->ast);
	if (g_signal == 130)
		return (free(minishell->p.pids), 130);
	if (minishell->p.count == 0 && is_builtin(s->words))
		return (only_builtin(minishell, &cmd));
	else
		execute_tree(minishell, &cmd);
	close_and_free(&minishell->p, &cmd);
	i = 0;
	while (i <= (minishell->p.count + minishell->p.o_count))
		s->code = waiting(minishell->p.pids[i++]);
	if (minishell->p.pids)
		free(minishell->p.pids);
	if (access(".heredoc", F_OK) == 0)
		unlink(".heredoc");
	return (s->code);
}
