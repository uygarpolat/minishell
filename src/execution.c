/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hpirkola <hpirkola@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 14:14:33 by hpirkola          #+#    #+#             */
/*   Updated: 2024/11/07 12:03:47 by hpirkola         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ast.h"

void	dupping(t_minishell *minishell, t_pipes *p, t_ast *s, int n)
{
	int	in;
	int	out;

	in = p->pipes[p->i][0];
	out = p->pipes[p->o][1];
	if (p->pipes)
	{
		if (n == 0)
		{
			if (!s->redir_target || s->redir_target->token->type != TOKEN_REDIR_OUT)
			{
				if (dup2(out, STDOUT_FILENO) == -1)
					error(minishell, "dup2 error\n");
			}
		}
		else if (n == minishell->p.count)
		{
			if (!s->redir_target || s->redir_target->type != TOKEN_REDIR_IN)
			{
				if (dup2(in, STDIN_FILENO) == -1)
					error(minishell, "dup2 error\n");
			}
		}
		else
		{
			if (!s->redir_target)
			{
				if (dup2(in, STDIN_FILENO) == -1 || dup2(out, STDOUT_FILENO) == -1)
					error(minishell, "dup2 error\n");
			}
			else
			{
				if (s->redir_target->type == TOKEN_REDIR_IN)
				{
					if (dup2(out, STDOUT_FILENO) == -1)
						error(minishell, "dup2 error\n");
				}
				else
				{
					if (dup2(in, STDIN_FILENO) == -1)
						error(minishell, "dup2 error\n");
				}
			}
		}
		close(in);
		close(out);
	}
}

int	dupping2(t_ast *s, char c)
{
	int	fd;
	
	if (c == 'i')
	{
		//open with read permissions
		fd = open(s->token->value, O_RDONLY);
		if (fd < 0)
			return (0);
		if (dup2(fd, STDIN_FILENO) == -1)
			printf("dup2 error\n");
	}
	else if (c == 'o')
	{
		//open with write permissions
		if (s->type == AST_REDIR_OUT)
			fd = open(s->token->value, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		else
			fd = open(s->token->value, O_APPEND | O_CREAT | O_WRONLY, 0644);
		if (fd < 0)
			return (0);
		if (dup2(fd, STDOUT_FILENO) == -1)
			printf("dup2 error\n");
	}
	return (1);
}

void	execute(t_ast *s, char ***envp, t_minishell *minishell, int n)
{
	int	i;
	t_ast	*redir;
	char	*path;

	minishell->p.pids[n] = fork();
	if (minishell->p.pids[n] == 0)
	{
		//redirect input and output if needed
		redir = s->redir_target;
		while (redir != NULL)
		{
			//do something
			if (redir->type == AST_REDIR_OUT || redir->type == AST_REDIR_APPEND)
			{
				if (!dupping2(redir, 'o'))
				{
					error(minishell, "Permission denied\n");
					exit(1);
				}
			}
			else if (redir->type == AST_REDIR_IN)
				if (!dupping2(redir, 'i'))
				{
					error(minishell, "Permission denied\n");
					exit(1);
				}
		}
		if (minishell->p.pipes)
		{
			if (n == minishell->p.count)
				minishell->p.o = minishell->p.count - 1;
			dupping(minishell, &minishell->p, s, n);
			//dupping(&minishell->p, minishell->p.pipes[minishell->p.i][0], minishell->p.pipes[minishell->p.o][1], n);
		}
		i = 0;
		while (i < minishell->p.count)
		{
			close(minishell->p.pipes[i][0]);
			close(minishell->p.pipes[i][1]);
			i++;
		}
		path = get_path(s->words, *envp, minishell);
		if (is_builtin(s->words))
		{
			execute_builtin(s->words, envp, minishell);
			exit(0);
		}
		execve(path, s->words, *envp);
		//FREE EVERYTHING HERE
		exit(126);
		
	}
}

int	count_pipes(t_ast *s)
{
	t_ast	*i;
	int	count;

	i = s;
	count = 0;
	while (i)
	{
		if (i->type == AST_PIPE)
			count++;
		i = i->right;
	}
	return (count);
}

int	pipeing(t_pipes *p)
{
	int	i;
	//int	j;

	p->i = 0;
	p->o = 0;
	i = -1;
	while (++i < p->count)
	{
		if (pipe(p->pipes[i]) < 0)
		{
			//j = 0;
			//while (j < i)
			//{
				//close(p->pipes[i][0]);
				//close(p->pipes[i][1]);
				//j++;
			//}
			return (0);
		}
	}
	return (1);
}

int	mallocing(t_pipes *p)
{
	int	i;
	//HANDLe MALLOC ERROR
	p->pipes = NULL;
	p->pids = NULL;
	if (p->count > 0)
	{
		p->pipes = malloc(sizeof(int *) * p->count);
		if (!p->pipes)
			return (0);
		i = -1;
		while (++i < p->count)
			p->pipes[i] = malloc(sizeof(int) * 2);
		p->pids = malloc(sizeof(int) * (p->count + 1));
		if (!p->pids)
			return (0);
	}
	else
	{
		p->pids = malloc(sizeof(int) * 1);
		if (!p->pids)
			return (0);
	}
	return (1);
}

void	close_and_free(t_pipes *p)
{
	int	i;

	i = 0;
	if (p->pipes)
	{
		while (i < p->count)
		{
			close(p->pipes[i][0]);
			close(p->pipes[i][1]);
			free(p->pipes[i]);
			i++;
		}
		free(p->pipes);
	}
}

int	waiting(int pid)
{
	int	status;
	int	code;
	
	status = 0;
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		code = WEXITSTATUS(status);
	else
		code = EXIT_FAILURE;
	return (code);
}
/*
void	in_out(t_pipes *p, t_ast *i)
{
	if (i->token.type == TOKEN_REDIR_OUT)
	{
		p->out = i->right;
	}
	else if (i->token.type == TOKEN_REDIR_IN)
	{
		p->in = i->right;
	}
	if (i->token.type == TOKEN_PIPE)
	{
		if (
	}
}
*/

int	execution(t_ast *s, char ***envp)
{
	t_minishell	minishell;
	int	n;
	int	j;
	
	minishell.ast = s;
	getcwd(minishell.pwd, sizeof(minishell.pwd));
	minishell.p.count = count_pipes(minishell.ast);
	if (!mallocing(&minishell.p) || !pipeing(&minishell.p))
	{
		error(&minishell, "malloc failed\n");
		return (1);
	}
	n = 0;
	if (minishell.p.count == 0 && is_builtin(s->words))
	{
			if (!execute_builtin(s->words, envp, &minishell))
			{	
				error(&minishell, "builtin failed\n");
				return (1);
			}
	}
	else
	{
		while (minishell.ast)
		{
			if (minishell.ast->type == AST_PIPE)
				execute(minishell.ast->left, envp, &minishell, n);
			else if (minishell.ast->type == AST_COMMAND)
				execute(minishell.ast, envp, &minishell, n);
			if (n > 0)
				minishell.p.i++;
			minishell.p.o++;
			n++;
			minishell.ast = minishell.ast->right;
		}
		close_and_free(&minishell.p);
		j = 0;
		while (j <= minishell.p.count)
			s->code = waiting(minishell.p.pids[j++]);
	}
	//i = -1;
	//free(minishell.pwd);
	free(minishell.p.pids);
	return (s->code);
}
