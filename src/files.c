/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   files.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hpirkola <hpirkola@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/04 14:39:14 by hpirkola          #+#    #+#             */
/*   Updated: 2025/01/20 12:45:01 by hpirkola         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ast.h"

void	dupping(t_minishell *minishell, t_pipes *p, t_put *cmd, int n)
{
	int	in;
	int	out;

	in = -1;
	out = -1;
	if (!cmd->infile && p->count > 0 && n > 0)
	{
		if (n % 2 == 0)
			in = p->pipes[1][0];
		else
			in = p->pipes[0][0];
	}
	else if (cmd->infile)
		in = cmd->in;
	if (!cmd->outfile && p->count > 0 && n < p->count)
	{
		if (n % 2 == 0)
			out = p->pipes[0][1];
		else
			out = p->pipes[1][1];
	}
	else if (cmd->outfile)
		out = cmd->out;
	if (/*(n < minishell->p.count || cmd->outfile)*/ out >= 0 && dup2(out, 1) == -1)
		error2(minishell, "dup2 error\n", cmd);
	if (/*(n > 0 || cmd->infile)*/ in >= 0 && dup2(in, 0) == -1)
		error2(minishell, "dup2 error\n", cmd);
	if (cmd->infile)
		close(cmd->in);
	if (cmd->outfile)
		close(cmd->out);
	if (in >= 0)
		close(in);
	if (out >= 0)
		close(out);
}

int	open_files(t_put *cmd)
{	
	if (cmd->infile)
	{
		if (cmd->in > 0)
			close(cmd->in);
		cmd->in = open(cmd->infile, O_RDONLY);
		if (cmd->in < 0)
			return (print_error(cmd->infile));
	}
	if (cmd->outfile)
	{
		if (cmd->out > 0)
			close(cmd->out);
		if (cmd->o_type == 'o')
			cmd->out = open(cmd->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		else
			cmd->out = open(cmd->outfile, O_APPEND | O_CREAT | O_WRONLY, 0644);
		if (cmd->out < 0)
			return (print_error(cmd->outfile));
	}
	return (1);
}

void	get_in_out(t_ast *s, t_put *cmd, t_minishell *minishell)
{
	t_ast	*temp;

	temp = s->redir_target;
	while (temp)
	{
		cmd->o_type = (temp->type == AST_REDIR_OUT) * 'o' \
				+ (temp->type == AST_REDIR_APPEND) * 'a';
		if (temp->type == AST_REDIR_IN)
			cmd->infile = temp->token->value;
		else if (temp->type == AST_REDIR_OUT)
			cmd->outfile = temp->token->value;
		else if (temp->type == AST_REDIR_APPEND)
			cmd->outfile = temp->token->value;
		else if (temp->type == AST_HEREDOC)
			cmd->infile = ".heredoc";
		if (!open_files(cmd))
		{
			file_error(minishell, cmd);
			exit (1);
		}
		temp = temp->redir_target;
	}
}

void	check_in_out(t_ast *s, t_minishell *minishell, t_put *file, int n)
{
	get_in_out(s, file, minishell);
	if (file->infile || file->outfile)
	{
		if (file->infile)
			file->stdin2 = dup(STDIN_FILENO);
		if (file->outfile)
			file->stdout2 = dup(STDOUT_FILENO);
		dupping(minishell, &minishell->p, file, n);
	}
	if (file->infile && file->in >= 0)
		close(file->in);
	if (file->outfile && file->out >= 0)
		close(file->out);
}
