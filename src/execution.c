/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hpirkola <hpirkola@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 14:14:33 by hpirkola          #+#    #+#             */
/*   Updated: 2024/10/24 13:37:39 by hpirkola         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ast.h"

void	dupping(t_pipes *p, int in, int out, int n)
{
	if (p->pipes)
	{
		if (n == 0)
		{
			if (dup2(out, STDOUT_FILENO) == -1)
				printf("dup2 error\n");
		}
		else if (n == p->count)
		{
			if (dup2(in, STDIN_FILENO) == -1)
				printf("dup2 error\n");
		}
		else
		{
			if (dup2(in, STDIN_FILENO) == -1 || dup2(out, STDOUT_FILENO) == -1)
				printf("dup2 error\n");
		}
		close(in);
		close(out);
	}
}

void	execute(t_ast *s, char **envp, t_pipes *p, int n)
{
	t_command	cmd;
	int	i;
	
	p->pids[n] = fork();
	if (p->pids[n] == 0)
	{
		//redirect input and output if needed
		if (p->pipes)
		{
			if (n == p->count)
				p->o = 1;
			dupping(p, p->pipes[p->i][0], p->pipes[p->o][1], n);
		}
		i = 0;
		while (i < p->count)
		{
			close(p->pipes[i][0]);
			close(p->pipes[i][1]);
			i++;
		}
		cmd.args = ft_split(s->token->value, ' ');
		cmd.path = get_path(cmd.args, envp);
		execve(cmd.path, cmd.args, envp);
		printf("error with execve\n");
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
		if (i->token->type == TOKEN_PIPE)
			count++;
		i = i->right;
	}
	return (count);
}

void	pipeing(t_pipes *p)
{
	int	i;
	int	j;

	p->i = 0;
	p->o = 0;
	i = -1;
	while (++i < p->count)
	{
		if (pipe(p->pipes[i]) < 0)
		{
			j = 0;
			while (j < i)
			{
				close(p->pipes[i][0]);
				close(p->pipes[i][1]);
				j++;
			}
			//error function here
		}
	}
}

void	mallocing(t_pipes *p)
{
	int	i;
	//HANDLe MALLOC ERROR
	p->pipes = NULL;
	p->pids = NULL;
	if (p->count > 0)
	{
		p->pipes = malloc(sizeof(int *) * p->count);
		if (!p->pipes)
			return ;
		i = -1;
		while (++i < p->count)
			p->pipes[i] = malloc(sizeof(int) * 2);
		p->pids = malloc(sizeof(int) * (p->count + 1));
		if (!p->pids)
			return ;
	}
	else
	{
		p->pids = malloc(sizeof(int) * 1);
		if (!p->pids)
			return ;
	}
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
int	execution(t_ast *s, char **envp)
{
	int	n;
	int	j;
	t_pipes	p;
	t_ast	*i;

	p.count = count_pipes(s);
	mallocing(&p);
	pipeing(&p);
	i = s;
	n = 0;
	while (i)
	{
		if (i->token->type == TOKEN_PIPE)
			execute(i->left, envp, &p, n);
		else if (i->token->type == TOKEN_WORD)
			execute(i, envp, &p, n);
		if (n > 0)
			p.i++;
		p.o++;
		n++;
		i = i->right;
	}
	close_and_free(&p);
	j = 0;
	while (j <= p.count)
		s->code = waiting(p.pids[j++]);
	free(p.pids);
	return (s->code);
}
