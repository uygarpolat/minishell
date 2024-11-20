/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_welcome.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/17 15:41:36 by upolat            #+#    #+#             */
/*   Updated: 2024/11/17 16:01:06 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../library/libft/libft.h"

int		execute_shell(char *input, int *code, char ***new_envp);

void	display_welcome_message(int *code, char **new_envp, int flag)
{
	char	*payload;
	char	*str;

	if (!flag)
		return ;
	payload = "echo \"\033[1;34m+--------------------\
--+\033[0m\n\033[1;34m| \033[1;37mWelcome to Minishell\
\033[0m\033[1;34m |\033[0m\n\033[1;34m+----------------------+\033[0m\"";
	str = ft_strdup("clear");
	if (str == NULL)
		return ;
	*code = execute_shell(str, code, &new_envp);
	if (*code)
		return ;
	str = ft_strdup(payload);
	if (str == NULL)
		return ;
	*code = execute_shell(str, code, &new_envp);
}
