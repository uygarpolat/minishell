/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/22 10:37:29 by upolat            #+#    #+#             */
/*   Updated: 2024/10/23 15:49:42 by upolat           ###   ########.fr       */
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
	//else if (type == TOKEN_REDIR_OUT || type == TOKEN_APPEND
	//	|| type == TOKEN_REDIR_IN || type == TOKEN_HEREDOC)
	//	return (4);
	return (-1);
}

int	find_matching_paren(t_tokens *tokens, int start, int end)
{
	int paren_count;
	int i;

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

// Function to create an AST node for a command (with arguments)
void	populate_command_node(t_tokens *tokens, t_ast_node *root, int start, int *end)
{
	int	i;

	i = start;
	root->type = AST_COMMAND;
	while (i < end)
	{
		if 	((tokens[start].type == TOKEN_REDIR_OUT) || (tokens[start].type == TOKEN_APPEND) || (tokens[start].type == TOKEN_REDIR_IN) || (tokens[start].type == TOKEN_HEREDOC))
		{
			if (root->redir_target != NULL)
				root->redir_target = root->redir_target->redir_target;
			root->redir_target = create_node(tokens);
i			root->redir_target->tokens->value = ft_strdup(tokens[i + 1].value); // Don't seg fault!!
		}
		else
		{
			root->token->value = ft_strjoin(root->token->value, tokens[i].value); // Is this leaking?
			root->left = NULL;
			root->right = NULL;
		}
		i++;
}

// Function to create an AST node for an operator
// (pipe, logical operators, redirections)
t_ast_node	*create_node(t_tokens *token)
{
	t_ast_node	*node;

	node = malloc(sizeof(t_ast_node)); // Do malloc check
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
	else if (token.type == TOKEN_WORD)
		node->type = AST_COMMAND;
	node->token = &token;
	node->left = NULL;
	node->right = NULL;
	node->redir_target = NULL;
	return (node);
}

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
	while (i <= end)
	{
		if ((i == start) && (tokens[start].type == TOKEN_OPEN_PAREN) && (end == find_matching_paren(tokens, start, end)))
		{
			i++;
			start++;
			end--;
		}
		if (tokens[i].type == TOKEN_OPEN_PAREN)
			i = find_matching_paren(tokens, i, end);
		if (i < 0)
		{
			ft_putstr_fd("Error: parenthesis mismatch.\n", 2);
			return (NULL);
		}
		prec = get_precedence(tokens[i].type);
		if (prec != -1 && prec < lowest_prec)
		{
			lowest_prec = prec;
			lowest_prec_pos = i;
		}
		i++;
	}

	if (lowest_prec_pos == -1)
		return (populate_command_node(tokens, root, start, &end));
	else if (lowest_prec <= 3)
	{
		root = create_node(tokens, start, &end);
		root->left = build_ast(tokens, start, lowest_prec_pos - 1);
		root->right = build_ast(tokens, lowest_prec_pos + 1, end);
	}
	else
	{
		root = create_node(tokens, start, &end);
		populate_command_node(tokens, root, start, &end);
	}
	return (root);
}

/*

	// If no operators are found, this is a simple command
	if (lowest_prec_pos == -1)
		return (create_command_node(tokens, start, &end)); // Create command node with arguments, also the base case
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
	node->redir_target = NULL;
	return (node);
}

   */
