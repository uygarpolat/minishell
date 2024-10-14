/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 11:16:11 by upolat            #+#    #+#             */
/*   Updated: 2024/10/14 17:30:04 by upolat           ###   ########.fr       */
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
#include "../includes/tokenizer.h"


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

int	is_space(char c)
{
	if (c = ' ')
		return (1);
	return (0);
}

int	is_seperator(char c)
{
	if (c == '<' || c == '>' || c == '&' || c == '|' ||
			c == '(' || c == ')')
		return (1);
	return (0);
}

t_tokens	*ft_tokenizer(char *input)
{
	int			i;
	char		*temp;
	t_tokens	*tokens = NULL;
	int			state ;

	temp = input;
	i = 0;

	while (*temp)
	{
		if (state = DEFAULT)
		{
			if (*temp)
			{
				if (ft_strlen(temp) > 0)
				{
					
				}
			}
			else if (*temp == '\'')
				state = SINGLE_Q_STATE;
			else if (*temp == '"')
				state = DOUBLE_Q_STATE;
			else
				ft_append_word
		}
		temp++;	
	}

	return (tokens);
}

int    main(void)
{
    char    	*input;
	t_tokens	*tokens;

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
		tokens = ft_tokenizer(input);
        free(input);
    }
    return (0);
}
