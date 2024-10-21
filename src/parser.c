/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 11:30:33 by upolat            #+#    #+#             */
/*   Updated: 2024/10/21 09:50:28 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../library/libft/libft.h"
#include "../includes/tokenizer.h"
#include "../includes/parser.h"

int	get_precedence(t_token_type type)
{
	if (type == TOKEN_AND)
		return (1);
	else if (type == TOKEN_OR)
		return (2);
	else if (type == TOKEN_PIPE)
		return (3);
	else if (type == TOKEN_REDIR_OUT || type == TOKEN_APPEND
		|| type == TOKEN_REDIR_IN || type == TOKEN_HEREDOC)
		return (4);
	return (-1); // Commands and other tokens have no precedence
}
/*
// Function to create an AST node for a command
t_ast_node	*create_command_node(t_tokens *tokens, int start)
{
	t_ast_node	*node;

	node = malloc(sizeof(t_ast_node)); // Do malloc check
	node->type = AST_COMMAND;
	node->token = &tokens[start]; // Store the command's token
	node->left = NULL;
	node->right = NULL;
	node->redir_target = NULL; // Not applicable for a command node
	return (node);
}
*/

// Function to create an AST node for a command (with arguments)
t_ast_node	*create_command_node(t_tokens *tokens, int start, int *end)
{
	t_ast_node	*arg_node;
	t_ast_node	*node;
	t_ast_node	*temp;
	int			i;

	node = malloc(sizeof(t_ast_node)); // Do a malloc check
	node->type = AST_COMMAND;
	node->token = &tokens[start]; // Store the command's token (e.g., "echo", "ls")
	node->left = NULL;
	node->right = NULL;
	node->redir_target = NULL; // Not applicable for a command node
	// Capture arguments (tokens following the command) until we hit a non-TOKEN_WORD
	i = start + 1;
	while (i <= *end && tokens[i].type == TOKEN_WORD)
	{
		// Store the argument as the right child (recursively handle multiple arguments)
		arg_node = malloc(sizeof(t_ast_node)); // Do a malloc check
		arg_node->type = AST_COMMAND;
		arg_node->token = &tokens[i]; // Store the argument's token (e.g., "hello", "-l")
		arg_node->left = NULL;
		arg_node->right = NULL;
		arg_node->redir_target = NULL;
		if (node->right == NULL)
			node->right = arg_node; // Attach the first argument
		else
		{
			// Attach subsequent arguments to the rightmost node
			temp = node->right;
			while (temp->right != NULL)
				temp = temp->right;
			temp->right = arg_node;
		}
		i++;
	}
	// Update the end position to the last argument token
	*end = i - 1;
	return (node);
}

// Function to create an AST node for an operator
// (pipe, logical operators, redirections)
t_ast_node	*create_operator_node(t_tokens token)
{
	t_ast_node	*node;

	node = malloc(sizeof(t_ast_node)); // Do malloc check
	// Set the AST node type based on the token type
	if (token.type == TOKEN_PIPE)
		node->type = AST_PIPE;
	else if (token.type == TOKEN_AND)
		node->type = AST_AND;
	else if (token.type == TOKEN_OR)
		node->type = AST_OR;
	else if (token.type == TOKEN_REDIR_OUT)
		node->type = AST_REDIR_OUT;
	else if (token.type == TOKEN_APPEND)
		node->type = AST_REDIR_APPEND;
	else if (token.type == TOKEN_REDIR_IN)
		node->type = AST_REDIR_IN;
	else if (token.type == TOKEN_HEREDOC)
		node->type = AST_HEREDOC;
	node->token = &token; // Store the operator's token
	node->left = NULL;
	node->right = NULL;
	node->redir_target = NULL; // Redirection targets will be handled separately
	return (node);
}
/*
// Build the AST based on operator precedence
t_ast_node	*build_ast(t_tokens *tokens, int start, int end)
{
	int			lowest_prec;
	int			lowest_prec_pos;
	int			i;
	int			prec;
	t_ast_node	*root;

	lowest_prec = 1000;
	lowest_prec_pos = -1;
	i = start;
	// Scan through the tokens to find the lowest precedence operator
	while (i <= end)
	{
		prec = get_precedence(tokens[i].type);
		if (prec != -1 && prec < lowest_prec)
		{
			lowest_prec = prec;
			lowest_prec_pos = i;
		}
		i++;
	}
	// If no operators are found, this is a simple command
	if (lowest_prec_pos == -1)
		return (create_command_node(tokens, start)); // Create command node
	// Create an operator node (logical operator, pipe, redirection, etc.)
	root = create_operator_node(tokens[lowest_prec_pos]);
	// Recursively build left and right subtrees for binary operators
	if (lowest_prec <= 3)
	{
		// Pipes, logical operators
		root->left = build_ast(tokens, start, lowest_prec_pos - 1);
		root->right = build_ast(tokens, lowest_prec_pos + 1, end);
	}
	else
	{
		// Redirections (one side only)
		root->left = build_ast(tokens, start, lowest_prec_pos - 1);
		root->redir_target = create_command_node(tokens, lowest_prec_pos + 1); // Redirection target
	}
	return (root);
}
*/

// Build the AST based on operator precedence, including handling arguments for commands
t_ast_node	*build_ast(t_tokens *tokens, int start, int end)
{
	int			lowest_prec;
	int			lowest_prec_pos;
	int			i;
	int			prec;
	t_ast_node	*root;

	lowest_prec = 1000;
	lowest_prec_pos = -1;
	i = start;
	// Scan through the tokens to find the lowest precedence operator
	while (i <= end)
	{
		prec = get_precedence(tokens[i].type);
		if (prec != -1 && prec < lowest_prec)
		{
			lowest_prec = prec;
			lowest_prec_pos = i;
		}
		i++;
	}
	// If no operators are found, this is a simple command
	if (lowest_prec_pos == -1)
		return (create_command_node(tokens, start, &end)); // Create command node with arguments
	// Create an operator node (logical operator, pipe, redirection, etc.)
	root = create_operator_node(tokens[lowest_prec_pos]);
	// Recursively build left and right subtrees for binary operators
	if (lowest_prec <= 3)
	{
		// Pipes, logical operators
		root->left = build_ast(tokens, start, lowest_prec_pos - 1);
		root->right = build_ast(tokens, lowest_prec_pos + 1, end);
	}
	else
	{
		// Redirections (one side only)
		root->left = build_ast(tokens, start, lowest_prec_pos - 1);
		root->redir_target = create_command_node(tokens, lowest_prec_pos + 1, &end); // Redirection target
	}
	return (root);
}
