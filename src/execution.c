/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 14:14:33 by hpirkola          #+#    #+#             */
/*   Updated: 2024/12/04 15:03:20 by hpirkola         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ast.h"
#include "../includes/signals.h"

void	execute(t_ast *s, char ***envp, t_minishell *minishell, int n, t_put *cmd)
{
	char	*path;

	minishell->p.pids[n] = fork();
	if (minishell->p.pids[n] != 0)
		return ;
	set_signals(s->code_parser, SIGNAL_CHILD); // Added by Uygar. Signals entering child process mode.
	get_in_out(s, cmd, minishell);
	if (minishell->p.pipes || cmd->infile || cmd->outfile)
	{
		if (n == minishell->p.count)
			minishell->p.o = minishell->p.count - 1;
		dupping(minishell, &minishell->p, cmd, n);
	}
	close_pipes(minishell);
	if (is_builtin(s->words))
		run_builtin(s, envp, minishell, n, cmd);
	if (!*s->words)
		exit(0);
	path = get_path(s->words, *envp);
	error_check(path, s);
	if (*(s->code_parser) == 130) // Added by Uygar
		exit(1); // comment from helmi: we need to clean everything since execve is not doing it for us
	execve(path, s->words, *envp);
	error(minishell, cmd);
	print_and_exit(strerror(errno), errno);
}

		/*
		Note from Uygar regarding heredoc:
		The following 2 lines are added by Uygar.
		*s->code_parser=130 happens if ctrl+C comes during heredoc.
		If there is a better place for these 2 lines, they can be moved.
		I chose before execve because I didn't know where else they could be.
		My understanding is nothing should go into execve if ctrl+C happens during heredoc.
		You can check against *s->code_parser to implement this elsewhere.
		Currently there are some problems:
		1) If heredoc input is "cat << END" and ctrl+C happens, echo $? returns 1,
		which mimicks Bash. But if heredoc input is "<< END", echo $? returns 0, which is wrong.
		Why does this happen?
		2) One more new line than bash is displayed when ctrl+C happens in heredoc mode.
		3) When ctrl+C comes, .heredoc file is not displayed on the screen, but what
		happens to .heredoc? We should make sure we are deleting and unlinking .heredoc
		after ctrl+C (I didn't do it).
		
		IMPORTANT: One very important assumption in my heredoc signal implementation is
		that heredoc happens in the parent process. If it is happening in a child process,
		we should talk. My understanding is that it must happen in parent process.
		*/

int	and_or(t_minishell *minishell, char ***envp, int n, t_put *cmd)
{
	int	status;

	if (minishell->ast->type == AST_AND)
	{	
		execute(minishell->ast->left, envp, minishell, n, cmd);
		while (minishell->ast->type == AST_AND)
		{
			status = 0;
			waitpid(minishell->p.pids[n], &status, 0);
			if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
			{
				if (minishell->ast->right->type == AST_COMMAND)
					execute(minishell->ast->right, envp, minishell, ++n, cmd);
				else
					execute(minishell->ast->right->left, envp, minishell, ++n, cmd);
			}
			else
			{
				while (minishell->ast->right->type == AST_AND)
					minishell->ast = minishell->ast->right;
				break ;
			}
			minishell->ast = minishell->ast->right;
		}
	}
	else if (minishell->ast->type == AST_OR)
	{
		execute(minishell->ast->left, envp, minishell, n, cmd);
		while (minishell->ast->type == AST_OR)
		{
			status = 0;
			waitpid(minishell->p.pids[n], &status, 0);
			if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
			{
				if (minishell->ast->right->type == AST_COMMAND)
					execute(minishell->ast->right, envp, minishell, ++n, cmd);
				else
					execute(minishell->ast->right->left, envp, minishell, ++n, cmd);
			}
			else
			{
				while (minishell->ast->right->type == AST_OR)
					minishell->ast = minishell->ast->right;
				break ;
			}
			minishell->ast = minishell->ast->right;
		}
	}
	return (n);
}

void	execute_tree(t_minishell *minishell, char ***envp, t_put *cmd)
{
	int	n;

	n = 0;
	while (minishell->ast)
	{
		if (minishell->ast->type == AST_PIPE)
			execute(minishell->ast->left, envp, minishell, n, cmd);
		else if (minishell->ast->type == AST_COMMAND)
			execute(minishell->ast, envp, minishell, n, cmd);
		else if (minishell->ast->type == AST_AND || minishell->ast->type == AST_OR)
			n = and_or(minishell, envp, n, cmd);
		if (n > 0)
			minishell->p.i++;
		minishell->p.o++;
		n++;
		minishell->ast = minishell->ast->right;
	}
}

void	initialize(t_put *cmd, t_minishell *minishell, t_ast *s)
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
	s->code = 0;

}

int	execution(t_ast *s, char ***envp)
{
	t_minishell	minishell;
	int			i;
	t_put		cmd;

	initialize(&cmd, &minishell, s);
	getcwd(minishell.pwd, sizeof(minishell.pwd)); //this needs to be done in the main
	if (!mallocing(&minishell.p) || !pipeing(&minishell.p))
	{
		error2(&minishell, "malloc failed\n", &cmd);
		return (1);
	}
	check_here(minishell.ast, envp);
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
