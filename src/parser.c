/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/22 10:37:29 by upolat            #+#    #+#             */
/*   Updated: 2024/10/28 23:39:46 by upolat           ###   ########.fr       */
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

void *free_void(void **var, void *return_value)
{
	if (var && *var)
	{
		free(*var);
		*var = NULL;
	}
	return (return_value);
}

void	free_ast(t_ast *node)
{
	if (!node)
		return ;
	if (node->token)
	{
		free_void((void**)&node->token->value, NULL);
		free_void((void**)&node->token, NULL);
	}
	if (node->left)
		free_ast(node->left);
	if (node->right)
		free_ast(node->right);
	if (node->redir_target)
		free_ast(node->redir_target);
	free_void((void**)&node, NULL);
}

t_tokens *copy_token(t_tokens *token)
{
	t_tokens *new_token;

   	new_token = malloc(sizeof(t_tokens));
	if (new_token == NULL)
		return (NULL);
	new_token->value = ft_strdup(token->value);
	if (new_token->value == NULL)
		return ((t_tokens* )free_void((void**)&new_token, NULL));
	new_token->type = token->type;
	return (new_token);
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

t_ast	*create_node(t_tokens *token)
{
	t_ast	*node;

	node = malloc(sizeof(t_ast));
	if (node == NULL)
		return (NULL);
	assign_token_type(node, token);
	node->token = copy_token(token);
	if (node->token == NULL)
		return ((t_ast* )free_void((void**)&node, NULL));
	node->left = NULL;
	node->right = NULL;
	node->redir_target = NULL;
	return (node);
}

int	redirection_node_creator(t_tokens *tokens, t_ast *root, int *i)
{
	t_ast	*temp;
	t_ast	*new_redir_node;
	
 	if (root->token->value)
	{	
		free(root->token->value);
		root->token->value = NULL;
	}
	new_redir_node = create_node(&tokens[*i]);
	if (new_redir_node == NULL)
		return (-1);
	free(new_redir_node->token->value);
	new_redir_node->token->value = NULL;
	new_redir_node->token->value = ft_strdup(tokens[++(*i)].value);
	if (new_redir_node->token->value == NULL)
		return (free_ast(new_redir_node), -1);
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

int	concatenate_commands(char **str, t_tokens *tokens, int *i)
{
	if (*str != NULL)
	{
		*str = ft_strjoin_free(*str, " ");
		if (*str == NULL)
			return (-1);
		*str = ft_strjoin_free(*str, tokens[*i].value);
		if (*str == NULL)
			return (-1);
	}
	else
	{
		*str = ft_strdup(tokens[*i].value);
		if (*str == NULL)
			return (-1);
	}
	return (0);
}

int	identify_token(t_token_type type)
{
	if ((type == TOKEN_REDIR_OUT) || (type == TOKEN_APPEND) || (type == TOKEN_REDIR_IN) || (type == TOKEN_HEREDOC))
		return (1);
	return (0);

}
int	populate_command_node(t_tokens *tokens, t_ast *root, int start, int *end)
{
	int		i;
	char	*str;

	str = NULL;
	i = start - 1;
	root->type = AST_COMMAND;
	while (++i <= *end)
	{
		if (identify_token(tokens[i].type))
			redirection_node_creator(tokens, root, &i);
		else if (concatenate_commands(&str, tokens, &i) == -1)
			return (-1); // Gotta free str as well.
	}
	if (str)
	{
		free_void((void**)&root->token->value, NULL);
		root->token->value = ft_strdup(str);
		free_void((void**)&str, NULL);
	}
	return (0);
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
