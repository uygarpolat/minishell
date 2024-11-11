/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hpirkola <hpirkola@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 11:05:16 by hpirkola          #+#    #+#             */
/*   Updated: 2024/11/11 16:15:37 by hpirkola         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ast.h"

void	error(t_minishell *minishell)
{
	//ft_putstr_fd(cmd->args[0], 2);
	//ft_putstr_fd(": ", 2);
	//ft_putstr_fd(str, 2);
	//if (minishell->ast)
	//	free_ast(&minishell->ast);
	if (minishell->p.pipes)
		close_and_free(&minishell->p);
	if (minishell->p.pids)
		free(minishell->p.pids);
}

void	error2(t_minishell *minishell, char *str)
{
	ft_putchar_fd(' ', 2);
	ft_putstr_fd(str, 2);
	//if (minishell->ast)
		//free_ast(&minishell->ast);
	if (minishell->p.pipes)
		close_and_free(&minishell->p);
	if (minishell->p.pids)
		free(minishell->p.pids);
}
