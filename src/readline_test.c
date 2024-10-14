/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   readline_test.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 11:16:11 by upolat            #+#    #+#             */
/*   Updated: 2024/10/12 20:27:55 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <readline/history.h>
#include "../../library/libft/libft.h"


void    handle_sigint()
{
    write(STDOUT_FILENO, "\n", 1);
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
}

void    handle_sigquit()
{
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
}
/*
char	**ft_tokenizer(char *input)
{
	char	**tokens;
}
*/
int    main(void)
{
    char    *input;
	//char	**tokens;

    signal(SIGINT, handle_sigint);
    signal(SIGQUIT, handle_sigquit);
    while (1)
    {
        input = readline("minishell> ");
        if (!input)
            break ;
        if (!ft_strncmp(input, "exit", 5))
            break ;
        if (*input)
            add_history(input);
        printf("%s\n", input);
		//tokens = ft_tokenizer(input);
        free(input);
    }
    return (0);
}
