/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/04 14:29:27 by hpirkola          #+#    #+#             */
/*   Updated: 2025/01/30 19:02:44 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/ast.h"
#include "../../includes/signals.h"

static int	count_docs(t_ast *ast)
{
	t_ast	*temp;
	t_ast	*temp2;
	int		count;

	temp = ast;
	count = 0;
	while (temp)
	{
		temp2 = ast->redir_target;
		while (temp2)
		{
			if (temp2->type == AST_HEREDOC)
				count++;
			temp2 = temp2->redir_target;
		}
		temp = temp->right;
	}
	return (count);
}

static void	too_many_docs(t_minishell *minishell, t_put *cmd)
{
	error(minishell, cmd);
	free_ast(&minishell->ast);
	ft_putstr_fd("minishell: maximum here-document count exceeded\n", 2);
	exit(2);
}

void	check_here(t_minishell *minishell, t_put *cmd)
{
	t_ast	*ast;
	int		count;
	int		i;
	int		j;

	ast = minishell->ast;
	i = 0;
	j = 0;
	count = count_docs(minishell->ast);
	if (count > 16)
		too_many_docs(minishell, cmd);
	while (ast)
	{
		if (ast->type == AST_COMMAND)
			here_loop(minishell, ast, cmd, &i);
		else if (ast->type == AST_PIPE)
			here_loop(minishell, ast->left, cmd, &i);
		ast = ast->right;
		cmd->cmd_fd[j++] = i - 1;
		if (g_signal == SIGINT || g_signal == SIGQUIT)
			break;
	}
}

void	here_loop(t_minishell *minishell, t_ast *ast, t_put *cmd, int *i)
{
	t_ast	*temp;
	int		flag;

	temp = ast->redir_target;
	flag = 0;
	while (temp)
	{
		if (temp->type == AST_HEREDOC)
		{
			minishell->here = 1;
			if (flag > 0)
				unlink(cmd->heredocs[*i - 1]);
			if (here(temp->token, minishell->ast, cmd, i) == -1)
				put_heredoc_ctrl_d(temp->token->value);
			*i += 1;
			flag++;
		}
		if (*ast->code_parser == 130)
			break ;
		temp = temp->redir_target;
	}
}

int	here(t_tokens *token, t_ast *ast, t_put *cmd, int *i)
{
	int			fd;
	const int	len = ft_strlen(token->value);
	char		*buf;

	fd = open(cmd->heredocs[*i], O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (fd < 0)
		return (0);
	set_signals(ast->code_parser, SIGNAL_HEREDOC);
	while (1)
	{
		buf = readline("> ");
		if (!buf)
			return (close(fd), -1);
		if (g_signal == SIGINT || g_signal == SIGQUIT)
		{
			*ast->code_parser = 128 + g_signal;
			break ;
		}
		if (!ft_strncmp(token->value, buf, len + 1))
			break ;
		heredoc_echo(fd, buf);
	}
	set_signals(ast->code_parser, SIGNAL_PARENT);
	free_void((void **)&buf, NULL);
	return (close(fd), 1);
}
