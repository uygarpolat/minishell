/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 11:16:11 by upolat            #+#    #+#             */
/*   Updated: 2024/10/15 10:31:33 by upolat           ###   ########.fr       */
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
	if (c == ' ')
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

ft_realloc_tokens_when_full(t_tokens **tokens)
{
	int	i;
	int	size;

	size = 10;
	i = 0;
	while (tokens[i])
		i++;
	if (i = 0 || i = size)
	{
		tokens = 
	}
}

void	handle_seperator(char **input, t_tokens **tokens)
{
	
}

void	handle_word(char **input, t_tokens **tokens)
{
	
}

t_tokens	*ft_tokenizer(char *input)
{
	int			initial_capacity;
	int			current_capacity;
	t_tokens	*tokens = NULL;

	initial_capacity = 10;
	current_capacity = 0;
	while (*input)
	{
		while (ft_strchr(" \t\n", *input) && *input)
			input++;
		if (is_seperator(*input))
			handle_seperator(&input, &tokens);
		else
			handle_word(&input, &tokens);
		current_capacity++;
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
