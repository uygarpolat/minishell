/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 14:14:33 by hpirkola          #+#    #+#             */
/*   Updated: 2025/01/16 14:06:58 by hpirkola         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ast.h"
#include "../includes/signals.h"

int	ft_pipe(t_pipes *p, int n)
{
	if (n % 2 == 0 && n < p->count && p->count > 0)
	{
		if (pipe(p->pipes[0]) < 0)
		{
			ft_putstr_fd("Pipe failed\n", 2);
			if (p->count > 1)
			{
				close(p->pipes[1][0]);
				close(p->pipes[1][1]);
			}
			return (0);
		}
		if (n > 0)
			close(p->pipes[1][1]);
	}
	else if (n < p->count && p->count > 0)
	{
		if (pipe(p->pipes[1]) < 0)
		{
			ft_putstr_fd("Pipe failed\n", 2);
			close(p->pipes[0][0]);
			close(p->pipes[0][1]);
			return (0);
		}
		close(p->pipes[0][1]);
	}
	return (1);
}

void	execute(t_ast *s, char ***envp, t_minishell *minishell, int n, t_put *cmd)
{
	char	*path;

	if (!ft_pipe(&minishell->p, n))
		return ; //pipe error
	minishell->p.pids[n] = fork();
	if (minishell->p.pids[n] != 0)
		return ;
	set_signals(s->code_parser, SIGNAL_CHILD);
	get_in_out(s, cmd, minishell);
	if (minishell->p.pipes || cmd->infile || cmd->outfile)
		dupping(minishell, &minishell->p, cmd, n);
	if (minishell->p.count)
		close_pipes(minishell, n);
	if (is_builtin(s->words))
		run_builtin(s, envp, minishell, n, cmd);
	if (!*s->words)
		exit(0);
	path = get_path(s->words, *envp);
	error_check(path, s, minishell, *envp, cmd);
	if (g_signal == 130)
		exit(130);
	execve(path, s->words, *envp);
	error(minishell, cmd, envp);
	print_and_exit(s->words[0], strerror(errno), errno, minishell);
}
/*
void	execute_no_pipes(t_ast *s, char ***envp, t_minishell *minishell, int n, t_put *cmd)
{
	char	*path;

	//ft_pipes(&minishell->pipes, n);
	minishell->p.pids[n] = fork();
	if (minishell->p.pids[n] != 0)
		return ;
	set_signals(s->code_parser, SIGNAL_CHILD); // Added by Uygar. Signals entering child process mode.
	get_in_out(s, cmd, minishell);
	if (minishell->p.pipes || cmd->infile || cmd->outfile)
	{
		//if (n == minishell->p.count)
			//minishell->p.o = minishell->p.count - 1;
		if (minishell->p.pipes)
			close(minishell->p.pipes[minishell->p.o][1]);
		if (cmd->infile)
		{
			//dup2 input from infile
			if (dup2(cmd->in, 0) == -1)
				error2(minishell, "dup2 error\n", cmd);
			close(cmd->in);
		}
		if (n == 1)
		{
			//else if (s->left && s->left->type == AST_PIPE)
			if (dup2(minishell->p.pipes[minishell->p.i][0], 1))
				error2(minishell, "dup2 error\n", cmd);
		}
		if (cmd->outfile)
		{
			if (dup2(cmd->out, 1) == -1)
				error2(minishell, "dup2, error\n", cmd);
			close(cmd->in);
		}
	}
	close_pipes(minishell, n);
	if (is_builtin(s->words))
		run_builtin(s, envp, minishell, n, cmd);
	if (!*s->words)
		exit(0);
	path = get_path(s->words, *envp);
	error_check(path, s, minishell, *envp, cmd);
	if (g_signal == 130) // Added by Uygar
	{
		error(minishell, cmd, envp);
		exit(130); // comment from helmi: we need to clean everything since execve is not doing it for us
	}
	execve(path, s->words, *envp);
	error(minishell, cmd, envp);
	print_and_exit(s->words[0], strerror(errno), errno, minishell);
}*/
/*
int	and_or(t_minishell *minishell, char ***envp, int n, t_put *cmd)
{
	int		status;
	t_ast	*temp;
	int		i;
	int		j;

	temp = minishell->ast;
	i = minishell->p.o_count - 1;
	while (temp->left->type != AST_COMMAND)
		temp = temp->left;
	if (temp->type == AST_AND || temp->type == AST_OR)
		execute_no_pipes(temp->left, envp, minishell, n, cmd);
	else if (temp->type == AST_PIPE)
	{
		execute(temp->left, envp, minishell, n, cmd);
		if (n > 0)
			minishell->p.i++;
		minishell->p.o++;	
	}
	while (i >= 0)
	{
		status = 0;
		waitpid(minishell->p.pids[n], &status, 0);
		if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
		{
			if (temp->type == AST_AND)
			{
				if (temp->right->type == AST_COMMAND)
					execute_no_pipes(temp->right, envp, minishell, ++n, cmd);
				else if (temp->right->type == AST_PIPE)
				{
					execute(temp->right->left, envp, minishell, ++n, cmd);
					if (n > 1)
						minishell->p.i++;
					minishell->p.o++;
					//execute right side of pipe
					execute(temp->right->right, envp, minishell, ++n, cmd);
					minishell->p.i++;
					minishell->p.o++;
				}
			}
			if (temp->type == AST_OR && temp->right->type != AST_PIPE)
			{
				minishell->ast = temp->right;
				break ;
			}
			else if (temp->type == AST_PIPE)
			{
				//need a while loop for pipes here??
				if (temp->right->type == AST_COMMAND)
					execute_no_pipes(temp->right, envp, minishell, ++n, cmd);
				else
					execute(temp->right->left, envp, minishell, ++n, cmd);
				if (n > 1)
					minishell->p.i++;
				minishell->p.o++;
				//execute right side of pipe
				if (temp->right->type == AST_PIPE)
				{
					execute(temp->right->right, envp, minishell, ++n, cmd);
					minishell->p.i++;
					minishell->p.o++;
				}
			}
		}
		else if (temp->type == AST_OR)
		{
			if (temp->right->type == AST_COMMAND)
				execute_no_pipes(temp->right, envp, minishell, ++n, cmd);
			else if (temp->right->type == AST_PIPE)
			{
				execute(temp->right->left, envp, minishell, ++n, cmd);
				if (n > 1)
					minishell->p.i++;
				minishell->p.o++;
				//execute right side of pipe
				execute(temp->right->right, envp, minishell, ++n, cmd);
				minishell->p.i++;
				minishell->p.o++;
			}
		
		}
		else if (temp->type == AST_PIPE)
		{
			//need a while loop for pipes here??
			if (temp->right->type == AST_COMMAND)
				execute_no_pipes(temp->right, envp, minishell, ++n, cmd);
			else
				execute(temp->right->left, envp, minishell, ++n, cmd);
			if (n > 1)
				minishell->p.i++;
			minishell->p.o++;
			//execute right side of pipe
			if (temp->right->type == AST_PIPE)
			{
				execute(temp->right->right, envp, minishell, ++n, cmd);
				minishell->p.i++;
				minishell->p.o++;
			}
		}
		temp = minishell->ast;
		j = 0;
		while (j < i - 1)
		{
			temp = temp->left;
			j++;
		}
		i--;
	}
	if (minishell->ast)
		minishell->ast = minishell->ast->right;
	return (n);
}*/	

void	execute_tree(t_minishell *minishell, char ***envp, t_put *cmd)
{
	int		n;
	t_ast	*ast;

	ast = minishell->ast;
	n = 0;
	while (ast)
	{
		if (ast->type == AST_PIPE)
			execute(ast->left, envp, minishell, n, cmd);
		else if (ast->type == AST_COMMAND)
			execute(ast, envp, minishell, n, cmd);
		//else if (ast->type == AST_AND || ast->type == AST_OR)
			//n = and_or(minishell, envp, n, cmd);
		n++;
		if (ast)
			ast = ast->right;
	}
}

int	initialize(t_put *cmd, t_minishell *minishell, t_ast *s, t_tokens *tokens, t_capacity capacity)
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
	minishell->p.i = 0;
	minishell->p.o = 0;
	minishell->tokens = tokens;
	minishell->capacity = capacity;
	if (getcwd(minishell->pwd, sizeof(minishell->pwd)) == NULL)
		return (0);
	s->code = 0;
	return (1);
}

int	execution(t_ast *s, char ***envp, t_tokens *tokens, t_capacity capacity)
{
	t_minishell	minishell;
	int			i;
	t_put		cmd;

	if (!initialize(&cmd, &minishell, s, tokens, capacity))
	{
		ft_putstr_fd("getcwd failed\n", 2);
		return (1);
	}
	if (!mallocing(&minishell.p))
	{
		error2(&minishell, "malloc failed\n", &cmd);
		return (1);
	}
	check_here(minishell.ast, envp);
	if (g_signal == 130)
	{
		free(minishell.p.pids);
		return (130);
	}
	if (minishell.p.count == 0 && is_builtin(s->words))
		return (only_builtin(envp, &minishell, &cmd));
	else
		execute_tree(&minishell, envp, &cmd);
	close_and_free(&minishell.p, &cmd);
	i = 0;
	while (i <= (minishell.p.count + minishell.p.o_count))
		s->code = waiting(minishell.p.pids[i++]);
	if (minishell.p.pids)
		free(minishell.p.pids);
	if (access(".heredoc", F_OK) == 0)
		unlink(".heredoc");
	return (s->code);
}
