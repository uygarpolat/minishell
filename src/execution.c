/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hpirkola <hpirkola@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 14:14:33 by hpirkola          #+#    #+#             */
/*   Updated: 2024/10/21 16:56:56 by hpirkola         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ast.h"
#include <stdio.h>

void	dupping(t_pipes *p, int n)
{
	if (p->pipes)
	{
		if (n == 0)
		{
			if (dup2(p->pipes[1], STDOUT_FILENO) == -1)
				printf("dup2 error\n");
			close(p->pipes[1]);
		}
		else if (n == p->count)
		{
			if (dup2(p->pipes[n - 1], STDIN_FILENO) == -1)
				printf("dup2 error\n");
			close(p->pipes[n - 1]);
		}
		else
		{
			if (dup2(p->pipes[2 * n - 2], STDIN_FILENO) == -1 || dup2(p->pipes[2 * n + 1], STDOUT_FILENO) == -1)
				printf("dup2 error\n");
			close(p->pipes[2 * n - 2]);
			close(p->pipes[2 * n + 1]);
		}
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
			dupping(p, n);
		i = 0;
		while (i < p->count * 2)
			close(p->pipes[i++]);
		cmd.args = ft_split(s->token.value, ' ');
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
		if (i->token.type == TOKEN_PIPE)
			count++;
		i = i->right;
	}
	return (count);
}

void	pipeing(t_pipes *p)
{
	int	i;
	int	j;

	i = -1;
	while (++i < p->count)
	{
		if (pipe(p->pipes + (2 * i)) < 0)
		{
			j = 0;
			while (j < i * 2)
				close(p->pipes[j++]);
			//error function here
		}
	}
}

void	mallocing(t_pipes *p)
{
	//HANDLe MALLOC ERROR
	p->pipes = malloc(sizeof(int *) * p->count * 2);
	if (!p->pipes)
		return ;
	p->pids = malloc(sizeof(int) * p->count);
	if (!p->pids)
		return ;
}

void	close_and_free(t_pipes *p)
{
	int	i;

	if (p->pipes)
	{
		while (i < p->count * 2)
			close(p->pipes[i++]);
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
		if (i->token.type == TOKEN_PIPE)
			execute(i->left, envp, &p, n++);
		else
			execute(i, envp, &p, n++);
		i = i->right;
	}
	close_and_free(&p);
	j = 0;
	while (j <= p.count)
		s->code = waiting(p.pids[j++]);
	//free(p.pids);
	return (s->code);
}
