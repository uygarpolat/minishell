/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 11:49:03 by upolat            #+#    #+#             */
/*   Updated: 2024/11/16 12:39:11 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/ast.h"

static int	get_precedence(t_token_type type)
{
	if (type == TOKEN_AND)
		return (1);
	else if (type == TOKEN_OR)
		return (1);
	else if (type == TOKEN_PIPE)
		return (3);
	return (-1);
}

int	find_matching_paren(t_tokens *tokens, int start, int end)
{
	int	paren_count;
	int	i;

	paren_count = 0;
	i = start;
	while (i <= end)
	{
		if (tokens[i].type == TOKEN_OPEN_PAREN)
			paren_count++;
		else if (tokens[i].type == TOKEN_CLOSE_PAREN)
		{
			paren_count--;
			if (paren_count < 0)
				return (-1);
			if (paren_count == 0)
				return (i);
		}
		i++;
	}
	return (-1);
}

void	assign_token_type(t_ast *node, t_tokens *token)
{
	if (token->type == TOKEN_PIPE)
		node->type = AST_PIPE;
	else if (token->type == TOKEN_AND)
		node->type = AST_AND;
	else if (token->type == TOKEN_OR)
		node->type = AST_OR;
	else if (token->type == TOKEN_REDIR_OUT)
		node->type = AST_REDIR_OUT;
	else if (token->type == TOKEN_APPEND)
		node->type = AST_REDIR_APPEND;
	else if (token->type == TOKEN_REDIR_IN)
		node->type = AST_REDIR_IN;
	else if (token->type == TOKEN_HEREDOC)
		node->type = AST_HEREDOC;
	else if (token->type == TOKEN_WORD)
		node->type = AST_COMMAND;
}

int	establish_lowest_precedence(t_tokens *tokens, t_precedence *p)
{
	while (p->i <= p->end)
	{
		while ((p->i == p->start) && (tokens[p->start].type == TOKEN_OPEN_PAREN)
			&& (p->end == find_matching_paren(tokens, p->start, p->end)))
		{
			p->i++;
			p->start++;
			p->end--;
		}
		if (tokens[p->i].type == TOKEN_OPEN_PAREN
			|| tokens[p->i].type == TOKEN_CLOSE_PAREN)
			p->i = find_matching_paren(tokens, p->i, p->end);
		if (p->i < 0)
			return (-1);
		p->prec = get_precedence(tokens[p->i].type);
		if (p->prec != -1 && p->prec < p->lowest_prec)
		{
			p->lowest_prec = p->prec;
			p->lowest_prec_pos = p->i;
		}
		p->i++;
	}
	return (0);
}

int	identify_token(t_token_type type)
{
	if ((type == TOKEN_REDIR_OUT) || (type == TOKEN_APPEND)
		|| (type == TOKEN_REDIR_IN) || (type == TOKEN_HEREDOC))
		return (1);
	return (0);
}
