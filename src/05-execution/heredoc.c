/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/04 14:29:27 by hpirkola          #+#    #+#             */
/*   Updated: 2025/01/22 19:42:13 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/ast.h"
#include "../../includes/signals.h"

void	check_here(t_ast *s)
{
	t_ast	*ast;

	ast = s;
	while (ast)
	{
		if (ast->type == AST_COMMAND)
			here_loop(s, ast);
		else if (ast->type == AST_AND || ast->type == AST_OR)
		{
			here_loop(s, ast);
			ast = ast->right;
		}
		else if (ast->type == AST_PIPE)
			here_loop(s, ast->left);
		ast = ast->right;
	}
}

void	here_loop(t_ast *s, t_ast *ast)
{
	t_ast	*temp;

	temp = ast->redir_target;
	while (temp)
	{
		if (temp->type == AST_HEREDOC)
			here(temp->token, s);
		temp = temp->redir_target;
	}
	if (ast->type == AST_AND || ast->type == AST_OR)
	{
		if (ast->right->type == AST_COMMAND)
			temp = ast->right->redir_target;
		else
			temp = ast->right->left->redir_target;
		while (temp)
		{
			if (temp->type == AST_HEREDOC)
				here(temp->token, s);
			temp = temp->redir_target;
		}
	}
}

int	here(t_tokens *token, t_ast *ast)
{
	int			fd;
	const int	len = ft_strlen(token->value);
	char		*buf;

	fd = open(".heredoc", O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (fd < 0)
		return (0);
	set_signals(ast->code_parser, SIGNAL_HEREDOC);
	while (1)
	{
		buf = readline("> ");
		if (g_signal == 130 || !buf)
		{
			*ast->code_parser = 130;
			break ;
		}
		if (!ft_strncmp(token->value, buf, len + 1))
			break ;
		write(fd, buf, ft_strlen(buf));
		write(fd, "\n", 1);
		free_void((void **)&buf, NULL);
	}
	set_signals(ast->code_parser, SIGNAL_PARENT);
	free_void((void **)&buf, NULL);
	return (close(fd), 1);
}
