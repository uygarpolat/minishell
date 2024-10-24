/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/22 10:37:29 by upolat            #+#    #+#             */
/*   Updated: 2024/10/24 15:31:36 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../library/libft/libft.h"
#include "../includes/tokenizer.h"
#include "../includes/ast.h"

int	get_precedence(t_token_type type)
{
	if (type == TOKEN_AND)
		return (1);
	else if (type == TOKEN_OR)
		return (2);
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

t_ast	*create_node(t_tokens *token)
{
	t_ast	*node;

	node = malloc(sizeof(t_ast)); // Do malloc check
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
	node->token = token;
	node->left = NULL;
	node->right = NULL;
	node->redir_target = NULL;
	return (node);
}

char	*beautify_token(char *str)
{
	char	*result;

	if (ft_strchr(str, '\'') <= ft_strchr(str, '"')) // If single quote comes first
		// do something
	else // If double quotes comes first
		// do something else
}

void	populate_command_node(t_tokens *tokens, t_ast *root, int start, int *end)
{
	int		i;
	char	*str;
	char	*str_modified;

	str = NULL;
	i = start;
	root->type = AST_COMMAND;
	while (i <= *end)
	{
		if ((tokens[i].type == TOKEN_REDIR_OUT) || (tokens[i].type == TOKEN_APPEND)
			|| (tokens[i].type == TOKEN_REDIR_IN) || (tokens[i].type == TOKEN_HEREDOC))
		{
			t_ast	*new_redir_node = create_node(&tokens[i]); // Refactor this, hence the incorrect decleration placement
			new_redir_node->token->value = ft_strdup(tokens[++i].value);
			if (root->redir_target == NULL)
				root->redir_target = new_redir_node;
			else
			{
				t_ast	*temp = root->redir_target; // Refactor this, hence the incorrect decleration placement
				while (temp->redir_target != NULL)
					temp = temp->redir_target;
				temp->redir_target = new_redir_node;
			}
		}
		else
		{
			char	*str_modified;

			str_modified = beautify_token(tokens[i].value);
			if (str_modified == NULL)
				return ; // Do a proper malloc check
			if (str != NULL)
			{
				str = ft_strjoin(str, " "); // Is this leaking?
				str = ft_strjoin(str, str_modified); // Is this leaking?
			}
			else
				str = ft_strdup(str_modified);
			free(str_modified);
		}
		root->left = NULL;
		root->right = NULL;
		i++;
	}
	root->token->value = ft_strdup(str);
	free(str);
}

t_ast	*build_ast(t_tokens *tokens, int start, int end)
{
	int			lowest_prec;
	int			lowest_prec_pos;
	int			i;
	int			prec;
	t_ast	*root;

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
	{
		root = create_node(&tokens[start]);
		populate_command_node(tokens, root, start, &end);
		return (root);
	}
	if (lowest_prec <= 3 && lowest_prec >= 0)
	{
		root = create_node(&tokens[lowest_prec_pos]);
		root->left = build_ast(tokens, start, lowest_prec_pos - 1);
		root->right = build_ast(tokens, lowest_prec_pos + 1, end);
	}
	else
	{
		root = create_node(&tokens[lowest_prec_pos]);
		populate_command_node(tokens, root, start, &end);
	}
	return (root);
}
