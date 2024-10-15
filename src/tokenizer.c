/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 11:16:11 by upolat            #+#    #+#             */
/*   Updated: 2024/10/15 17:30:35 by upolat           ###   ########.fr       */
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

t_tokens	*ft_realloc_tokens_when_full(t_tokens **tokens, t_capacity *capacity)
{
	t_tokens	*new_tokens;

	new_tokens = malloc(sizeof(t_tokens) * (capacity->capacity * 2));
	if (new_tokens == NULL)
	{
		free(tokens); // Also free the string stored in array of struct
		return (NULL);
	}

}

void	handle_seperator(char **input, t_tokens **tokens)
{
	char	*temp;

	temp = *input;
}

void	handle_word(char **input, t_tokens **tokens)
{
	char	*temp;

	temp = *input;
	if (*temp == '\'')
	{
		while (ft_strchr(temp, '\''))
			temp++;
	}
	else if (*temp == '"')
	{
		while (ft_strchr(temp, '\''))
			temp++;
	}
	else
	{
		while (ft_strchr(temp, ' '))
			temp++;
	}

}

t_tokens	*ft_tokenizer(char *input)
{
	t_capacity	capacity;
	t_tokens	*tokens = NULL;

	capacity.capacity = 10;
	capacity.current_size = 0;
	tokens = malloc(sizeof(t_tokens) * capacity.capacity);
	if (tokens == NULL)
		return (NULL);
	while (*input)
	{
		while (ft_strchr(" \t\n", *input) && *input)
			input++;
		if (capacity.capacity <= capacity.current_size)
			ft_realloc_when_tokens_full(&tokens, &capacity)
		if (is_seperator(*input))
			handle_seperator(&input, &tokens);
		else
			handle_word(&input, &tokens);
		capacity.current_size++;
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
