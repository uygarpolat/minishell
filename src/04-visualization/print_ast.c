/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_ast.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 01:54:46 by upolat            #+#    #+#             */
/*   Updated: 2024/11/16 02:35:05 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/ast.h"

void	print_indent(int level)
{
	int	indent_level;

	indent_level = -1;
	while (++indent_level < level)
		printf("  ");
}

void	print_node_type(t_ast *node)
{
	if (node->type == AST_COMMAND)
		printf("AST_COMMAND [TOKEN: ");
	else if (node->type == AST_PIPE)
		printf("AST_PIPE [TOKEN: |]\n");
	else if (node->type == AST_REDIR_OUT)
		printf("AST_REDIR_OUT [TOKEN: >]\n");
	else if (node->type == AST_REDIR_APPEND)
		printf("AST_REDIR_APPEND [TOKEN: >>]\n");
	else if (node->type == AST_REDIR_IN)
		printf("AST_REDIR_IN [TOKEN: <]\n");
	else if (node->type == AST_HEREDOC)
		printf("AST_HEREDOC [TOKEN: <<]\n");
	else if (node->type == AST_AND)
		printf("AST_AND [TOKEN: &&]\n");
	else if (node->type == AST_OR)
		printf("AST_OR [TOKEN: ||]\n");
	else
		printf("Unknown node type\n");
}

void	print_command_words(t_ast *node, int level)
{
	char	**temp_double_arr;

	temp_double_arr = node->words;
	while (*node->words)
	{
		printf("%s ", *(node->words));
		(node->words)++;
	}
	printf("]\n");
	node->words = temp_double_arr;
	while (*node->words)
	{
		print_indent(level + 1);
		printf("[TOKEN: %s]\n", *(node->words));
		(node->words)++;
	}
	node->words = temp_double_arr;
}

void	print_redirections(t_ast *node, int level)
{
	t_ast	*redir_node;
	char	*redir_symbol;

	redir_node = node->redir_target;
	while (redir_node != NULL)
	{
		print_indent(level + 1);
		redir_symbol = "?";
		if (redir_node->type == AST_REDIR_OUT)
			redir_symbol = ">";
		else if (redir_node->type == AST_REDIR_APPEND)
			redir_symbol = ">>";
		else if (redir_node->type == AST_REDIR_IN)
			redir_symbol = "<";
		else if (redir_node->type == AST_HEREDOC)
			redir_symbol = "<<";
		printf("REDIR: %s [TOKEN: %s]\n", redir_symbol,
			redir_node->token->value);
		redir_node = redir_node->redir_target;
	}
}

void	print_ast(t_ast *node, int level)
{
	if (node == NULL)
		return ;
	print_indent(level);
	print_node_type(node);
	if (node->type == AST_COMMAND)
		print_command_words(node, level);
	if (node->left != NULL)
		print_ast(node->left, level + 1);
	if (node->right != NULL)
		print_ast(node->right, level + 1);
	print_redirections(node, level);
}
