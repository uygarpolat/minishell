/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory_and_error.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 11:53:06 by upolat            #+#    #+#             */
/*   Updated: 2024/11/16 13:42:39 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/ast.h"

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
		return (error_handler(NULL, NULL, token->code, 1), NULL);
	new_token->value = ft_strdup(token->value);
	if (new_token->value == NULL)
	{
		error_handler(NULL, NULL, token->code, 1);
		return ((t_tokens *)free_void((void **)&new_token, NULL));
	}
	new_token->type = token->type;
	new_token->code = token->code;
	return (new_token);
}

int	cleanup_populate_command_node(t_ast **root, int *error_code)
{
	if (!*error_code)
		return (0);
	free_ast(root);
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
	if (tokens != NULL)
		*tokens->code = 2;
}
