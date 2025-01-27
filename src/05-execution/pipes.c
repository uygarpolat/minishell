/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/04 14:43:30 by hpirkola          #+#    #+#             */
/*   Updated: 2025/01/27 11:16:34 by hpirkola         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/ast.h"

void	close_pipes(t_minishell *minishell, int n)
{
	int	i;

	i = 0;
	if (minishell->p.count == 1 || n == 0)
	{
		close(minishell->p.pipes[0][0]);
		close(minishell->p.pipes[0][1]);
	}
	else
	{
		while (i < 2)
		{
			close(minishell->p.pipes[i][0]);
			close(minishell->p.pipes[i][1]);
			i++;
		}
	}
}

int	count_pipes(t_ast *s)
{
	t_ast	*i;
	int		count;

	i = s;
	count = 0;
	while (i->left && (i->type == AST_AND || i->type == AST_OR))
		i = i->left;
	while (i->type != AST_COMMAND)
	{
		if (i->type == AST_PIPE)
			count++;
		i = i->right;
	}
	return (count);
}

/*
int	count_operators(t_ast *s)
{
	t_ast	*temp;
	t_ast	*i;
	int		count;

	i = s;
	temp = s;
	count = 0;
	while (i->type != AST_COMMAND)
	{
		if (i->type == AST_AND || i->type == AST_OR)
			temp = i;
		else if (i->left->type == AST_AND || i->left->type == AST_OR)
			temp = i->right;
		else if (i->right->type == AST_AND || i->right->type == AST_OR)
			temp = i->right;
		while (temp->type == AST_AND || temp->type == AST_OR)
		{
			temp = temp->left;
			count++;
		}
		i = i->right;
	}
	return (count);
}*/

int	malloc_check(t_pipes *p)
{
	if (p->count == 1)
		p->pipes = malloc(sizeof(int *) * 1);
	else
		p->pipes = malloc(sizeof(int *) * 2);
	if (!p->pipes)
		return (0);
	return (1);
}

int	mallocing(t_pipes *p)
{
	int	i;

	p->pipes = NULL;
	p->pids = NULL;
	if (p->count > 0)
	{
		if (!malloc_check(p))
			return (0);
		i = -1;
		while (++i < 2)
		{
			p->pipes[i] = malloc(sizeof(int) * 2);
			if (!p->pipes[i])
				return (0);
			p->pipes[i][0] = -1;
			p->pipes[i][1] = -1;
			if (p->count == 1)
				break ;
		}
	}
	p->pids = malloc(sizeof(int) * (p->count + 1));
	if (!p->pids)
		return (0);
	return (1);
}
