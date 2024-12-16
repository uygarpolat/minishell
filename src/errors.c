/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hpirkola <hpirkola@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 11:05:16 by hpirkola          #+#    #+#             */
/*   Updated: 2024/12/16 13:09:36 by hpirkola         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ast.h"

void	error(t_minishell *minishell, t_put *cmd)
{
	if (minishell->p.pipes)
		close_and_free(&minishell->p, cmd);
	if (minishell->p.pids)
		free(minishell->p.pids);
}

void	error2(t_minishell *minishell, char *str, t_put *cmd)
{
	ft_putchar_fd(' ', 2);
	ft_putstr_fd(str, 2);
	if (minishell->p.pipes)
		close_and_free(&minishell->p, cmd);
	if (minishell->p.pids)
		free(minishell->p.pids);
}

int	print_error(void)
{
	ft_putchar_fd(' ', 2);
	ft_putstr_fd(strerror(errno), 2);
	ft_putchar_fd('\n', 2);
	return (0);
}

void	print_and_exit(char *str, int code)
{
	ft_putstr_fd(str, 2);
	exit(code);
}

void	error_check(char *path, t_ast *s)
{
	struct stat	buf;

	if (!path)
	{
		if (!ft_strchr(s->words[0], '/'))
			print_and_exit(" command not found\n", 127);
		else if (stat(s->words[0], &buf) == 0 && access(s->words[0], X_OK) != 0)
			print_and_exit(" Permission denied\n", 126);
		else if (ft_strchr(s->words[0], '/'))
			print_and_exit(" No such file or directory\n", 127);
	}
	if (stat(path, &buf) == 0)
	{
		if (!ft_strncmp(s->words[0], "..", 3) || !ft_strncmp(s->words[0], "\0", 2))
			print_and_exit(" command not found\n", 127);
		if (S_ISDIR(buf.st_mode))
			print_and_exit(" Is a directory\n", 126);
		if (access(path, X_OK) != 0)
			print_and_exit(" Permission denied\n", 127);
	}
}
