/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hpirkola <hpirkola@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 11:05:16 by hpirkola          #+#    #+#             */
/*   Updated: 2025/01/08 14:00:21 by hpirkola         ###   ########.fr       */
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

int	print_error(char *str)
{
	ft_putstr_fd(str, 2);
	ft_putstr_fd(": ", 2);
	ft_putstr_fd(strerror(errno), 2);
	ft_putchar_fd('\n', 2);
	return (0);
}

void	print_and_exit(char *cmd, char *str, int code, t_ast *s)
{
	if (!cmd[0])
		ft_putstr_fd("''", 2);
	else
		ft_putstr_fd(cmd, 2);
	ft_putstr_fd(": ", 2);
	ft_putstr_fd(str, 2);
	free_ast(&s);
	exit(code);
}

void	error_check(char *path, t_ast *s, t_pipes *p, char	**envp)
{
	struct stat	buf;

	if (!path)
	{
		if (!ft_strchr(s->words[0], '/'))
			print_and_exit(s->words[0], "command not found\n", 127, s);
		else if (stat(s->words[0], &buf) == 0 && access(s->words[0], X_OK) != 0)
			print_and_exit(s->words[0], "Permission denied\n", 126, s);
		else if (ft_strchr(s->words[0], '/'))
			print_and_exit(s->words[0], "No such file or directory\n", 127, s);
	}
	if (stat(path, &buf) == 0)
	{
		if (!ft_strncmp(s->words[0], "..", 3) || !ft_strncmp(s->words[0], "\0", 2))
		{
			free_2d_array((void ***)&envp);
			free(p->pids);
			free(path);
			print_and_exit(s->words[0], "command not found\n", 127, s);
		}
		if (!ft_strncmp(s->words[0], ".", 2))
		{
			free_2d_array((void ***)&envp);
			free(p->pids);
			free(path);
			ft_putstr_fd("minishell: ", 2);
			print_and_exit(s->words[0], "filename argument required\n", 2, s);
		}
		if (S_ISDIR(buf.st_mode))
		{
			free_2d_array((void ***)&envp);
			free(p->pids);
			free(path);
			print_and_exit(s->words[0], "Is a directory\n", 126, s);
		}
		if (access(path, X_OK) != 0)
		{
			free_2d_array((void ***)&envp);
			free(p->pids);
			free(path);
			print_and_exit(s->words[0], "Permission denied\n", 127, s);
		}
	}
}
