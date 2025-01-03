/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hpirkola <hpirkola@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/04 14:43:30 by hpirkola          #+#    #+#             */
/*   Updated: 2024/12/20 12:21:34 by hpirkola         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ast.h"

void	close_pipes(t_minishell *minishell)
{
	int	i;

	i = 0;
	while (i < minishell->p.count)
	{
		close(minishell->p.pipes[i][0]);
		close(minishell->p.pipes[i][1]);
		i++;
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
}

int	pipeing(t_pipes *p)
{
	int	i;

	p->i = 0;
	p->o = 0;
	i = -1;
	while (++i < p->count)
	{
		if (pipe(p->pipes[i]) < 0)
			return (0);
	}
	return (1);
}

int	mallocing(t_pipes *p)
{
	int	i;

	p->pipes = NULL;
	p->pids = NULL;
	if (p->count > 0)
	{
		p->pipes = malloc(sizeof(int *) * p->count);
		if (!p->pipes)
			return (0);
		i = -1;
		while (++i < p->count)
		{
			p->pipes[i] = malloc(sizeof(int) * 2);
			if (!p->pipes[i])
				return (0);
		}
	}
	p->pids = malloc(sizeof(int) * (p->count + p->o_count + 1));
	if (!p->pids)
		return (0);
	return (1);
}
