/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/22 10:37:29 by upolat            #+#    #+#             */
/*   Updated: 2024/11/16 13:45:06 by upolat           ###   ########.fr       */
/*   Updated: 2024/10/28 13:13:09 by hpirkola         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/ast.h"

static t_ast	*create_node(t_tokens *token)
{
	t_ast	*node;

	node = malloc(sizeof(t_ast));
	if (node == NULL)
		return (error_handler(NULL, NULL, token->code, 1), NULL);
	assign_token_type(node, token);
	node->token = copy_token(token);
	if (node->token == NULL)
	{
		error_handler(NULL, NULL, token->code, 1);
		return ((t_ast *)free_void((void **)&node, NULL));
	}
	node->words = NULL;
	node->left = NULL;
	node->right = NULL;
	node->redir_target = NULL;
	node->code = 0;
	node->code_parser = node->token->code;
	return (node);
}

int	redirection_node_creator(t_tokens *tokens, t_ast *root, int *i)
{
	t_ast	*temp;
	t_ast	*new_redir_node;

	free_void((void **)&root->token->value, 0);
	new_redir_node = create_node(&tokens[*i]);
	if (new_redir_node == NULL)
		return (-1);
	free_void((void **)&new_redir_node->token->value, 0);
	new_redir_node->token->value = ft_strdup(tokens[++(*i)].value);
	if (new_redir_node->token->value == NULL)
		return (free_ast(&new_redir_node), -1);
	if (root->redir_target == NULL)
		root->redir_target = new_redir_node;
	else
	{
		temp = root->redir_target;
		while (temp->redir_target != NULL)
			temp = temp->redir_target;
		temp->redir_target = new_redir_node;
	}
	return (0);
}

static void	build_non_command_node(t_tokens *tokens, t_ast **root,
			t_precedence *p, int *error_code)
{
	*root = create_node(&tokens[p->lowest_prec_pos]);
	if (*root == NULL)
		*error_code = -1;
	if (!*error_code)
	{
		(*root)->left = build_ast(tokens, p->start,
				p->lowest_prec_pos - 1, *error_code);
		if ((*root)->left == NULL)
		{
			free_ast(root);
			*error_code = -1;
		}
	}
	if (!*error_code)
	{
		(*root)->right = build_ast(tokens, p->lowest_prec_pos + 1,
				p->end, *error_code);
		if ((*root)->right == NULL)
		{
			free_ast(&(*root)->left);
			free_ast(root);
			*error_code = -1;
		}
	}
}

static int	build_ast_error_check(t_tokens *tokens, t_precedence *p,
				int *error_code)
{
	p->lowest_prec = 1000;
	p->lowest_prec_pos = -1;
	p->i = p->start;
	if (p->start > p->end)
	{
		*error_code = -1;
		if (p->end > 0)
			p->start -= 1;
		syntax_error_near(tokens, p->start);
		return (-1);
	}
	return (0);
}

t_ast	*build_ast(t_tokens *tokens, int start, int end, int code)
{
	t_precedence	p;
	t_ast			*root;
	static int		error_code;

	root = NULL;
	error_code = code;
	p.start = start;
	p.end = end;
	if (build_ast_error_check(tokens, &p, &error_code) == -1)
		return (root);
	if (!error_code && establish_lowest_precedence(tokens, &p) == -1)
		return (NULL);
	if (!error_code && p.lowest_prec <= 3 && p.lowest_prec >= 0)
		build_non_command_node(tokens, &root, &p, &error_code);
	else if (!error_code)
	{
		root = create_node(&tokens[p.start]);
		if (root == NULL)
			error_code = -1;
		if (!error_code && populate_command_node(tokens,
				root, p.start, &(p.end)) == -1)
			free_ast(&root);
	}
	return (root);
}
