/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_utils_3.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 09:49:12 by upolat            #+#    #+#             */
/*   Updated: 2025/01/28 09:49:51 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/ast.h"
#include "../../includes/signals.h"

void	put_heredoc_ctrl_d(char *str)
{
	ft_putstr_fd("minishell: warning: here-document at" \
		"line 1 delimited by end-of-file", 2);
	ft_putstr_fd(" (wanted ", 2);
	ft_putstr_fd("`", 2);
	ft_putstr_fd(str, 2);
	ft_putstr_fd("')\n", 2);
}

void	heredoc_echo(int fd, char *buf)
{
	write(fd, buf, ft_strlen(buf));
	write(fd, "\n", 1);
	free_void((void **)&buf, NULL);
}
