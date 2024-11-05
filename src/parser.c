/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/22 10:37:29 by upolat            #+#    #+#             */
/*   Updated: 2024/11/06 00:06:47 by upolat           ###   ########.fr       */
/*   Updated: 2024/10/28 13:13:09 by hpirkola         ###   ########.fr       */
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
		return (1); // Is thos correct?
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

void	free_ast(t_ast **node)
{
	if (node == NULL || *node == NULL)
		return ;
	if ((*node)->token)
	{
		free_void((void **)&(*node)->token->value, NULL);
		free_void((void **)&(*node)->token, NULL);
	}
	if ((*node)->words)
		free_2d_array((void ***)&(*node)->words);
	if ((*node)->left)
		free_ast(&(*node)->left);
	if ((*node)->right)
		free_ast(&(*node)->right);
	if ((*node)->redir_target)
		free_ast(&(*node)->redir_target);
	free_void((void **)node, NULL);
}

t_tokens	*copy_token(t_tokens *token)
{
	t_tokens	*new_token;

	new_token = malloc(sizeof(t_tokens));
	if (new_token == NULL)
		return (NULL);
	new_token->value = ft_strdup(token->value);
	if (new_token->value == NULL)
		return ((t_tokens *)free_void((void **)&new_token, NULL));
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
		return ((t_ast *)free_void((void **)&node, NULL));
	node->words = NULL;
	node->left = NULL;
	node->right = NULL;
	node->redir_target = NULL;
	node->code = 0;
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
	if ((type == TOKEN_REDIR_OUT) || (type == TOKEN_APPEND)
		|| (type == TOKEN_REDIR_IN) || (type == TOKEN_HEREDOC))
		return (1);
	return (0);
}

int	cleanup_populate_command_node(t_ast **root, char **str, int *error_code)
{
	if (!*error_code)
		return (0);
	free_ast(root);
	free_void((void **)str, error_code);
	return (-1);
}

void	syntax_error_near(t_tokens *tokens, int loc)
{
	char	*str;

	if (loc == -1)
		str = "newline";
	else if (tokens[loc].value == NULL)
		str = "newline";
	else
		str = tokens[loc].value;
	ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
	ft_putstr_fd(str, 2);
	ft_putstr_fd("'\n", 2);
}

int	basic_command_node_error_handling(t_tokens *tokens, int start, int *end)
{
	int	i;
	int	k;

	i = start;
	k = *end;
	if (tokens[i].type != TOKEN_OPEN_PAREN)
	{
		while (++i <= *end)
		{
			if (tokens[i].type == TOKEN_OPEN_PAREN)
			{
				printf("Aborting because encountered a TOKEN_WORD before TOKEN_OPEN_PAREN, which is %s!\n", tokens[i - 1].value);
				return (syntax_error_near(tokens, i), -1);
			}
		}
	}
	else
	{
		k = find_matching_paren(tokens, i, k);
		if (k >= 0)
		{
			if (!identify_token(tokens[k + 1].type))
			{
				printf("Aborting because encountered a non-redirection token after TOKEN_CLOSE_PAREN, which is %s!\n", tokens[k + 1].value);
				return (syntax_error_near(tokens, k + 1), -1);
			}
		}
	}
	if (tokens[*end].type != TOKEN_WORD)
	{
		// This seg faults for instance for "echo hello world >",
		// because capacity is full and the next one is not NULL
		// (everything in capacity that is not malloced is set to NULL. See syntax_error_near to understand.
		printf("Aborting due to last token not being TOKEN_WORD, which was %s\n", tokens[*end].value);
		return (syntax_error_near(tokens, -1), -1); // The paramters before the refactor was (tokens, *end + 1).
	}
	return (0);
}

int	populate_command_node(t_tokens *tokens, t_ast *root, int start, int *end)
{
	int		i;
	char	*str;
	char	**temp;
	int		error_code;

	if (basic_command_node_error_handling(tokens, start, end))
	{
		error_code = -1;
		return (error_code);
	}
	root->words = ft_calloc(*end - start + 2, sizeof(char *));
	if (root->words == NULL)
		return (-1);
	root->words[*end - start + 1] = 0;
	temp = root->words;
	error_code = 0;
	str = NULL;
	i = start - 1;
	root->type = AST_COMMAND;
	while (++i <= *end && !error_code)
	{
		if (tokens[i].type == TOKEN_OPEN_PAREN
			|| tokens[i].type == TOKEN_CLOSE_PAREN)
			continue ;
		if (identify_token(tokens[i].type))
			error_code = redirection_node_creator(tokens, root, &i);
		else if (ft_strlen(tokens[i].value) > 0)
		{
			while (*root->words)
				root->words++;
			*root->words = ft_strdup(tokens[i].value);
			error_code = concatenate_commands(&str, tokens, &i);
		}
	}
	root->words = temp;
	if (str && !error_code)
	{
		free_void((void **)&root->token->value, NULL);
		root->token->value = ft_strdup(str);
		if (root->token->value == NULL)
			error_code = -1;
		free_void((void **)&str, NULL);
	}
	return (cleanup_populate_command_node(&root, &str, &error_code));
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
		//if (basic_command_node_error_handling(tokens, p->start, &p->end))
		//	return (-1); // This was intending to check if the last token is redirection. Refactored it, but is that feature still there?
		if (tokens[p->i].type == TOKEN_OPEN_PAREN
			|| tokens[p->i].type == TOKEN_CLOSE_PAREN)
			p->i = find_matching_paren(tokens, p->i, p->end);
		if (p->i < 0)
			return (ft_putstr_fd("Error: parenthesis mismatch.\n", 2), -1);
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

void	build_non_command_node(t_tokens *tokens, t_ast **root,
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

int	build_ast_error_check(t_tokens *tokens, t_precedence *p, int *error_code)
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

	error_code = code;
	root = NULL;
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
				root, p.start, &(p.end)) == -1) // Is this check really correct?
			free_ast(&root);
	}
	return (root);
}
