/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 11:16:11 by upolat            #+#    #+#             */
/*   Updated: 2024/10/15 23:57:58 by upolat           ###   ########.fr       */
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

void	print_tokens(t_tokens *tokens, t_capacity capacity)
{
	int	i;

	i = -1;
	while (++i < capacity.current_size)
		printf("token %d is: %s\n", i, tokens[i].value);
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
/*
t_tokens	*ft_realloc_tokens_when_full(t_tokens **tokens, t_capacity *capacity)
{
	t_tokens	*new_tokens;

	new_tokens = malloc(sizeof(t_tokens) * (capacity->max_size * 2));
	if (new_tokens == NULL)
	{
		free(tokens); // Also free the string stored in array of struct
		return (NULL);
	}
}
*/
/*
void	handle_seperator(char **input, t_tokens **tokens)
{
	char	*temp;

	temp = *input;
}
*/
void	handle_word(char **input, t_tokens *tokens, t_capacity *capacity)
{
	char	*temp;

	temp = *input;
	if (*temp == '\'')
	{
		while (*temp && ft_strchr(temp, '\''))
			temp++;
	}
	else if (*temp == '"')
	{
		while (*temp && ft_strchr(temp, '"'))
			temp++;
	}
	else
	{
		while (*temp && !ft_strchr(" \t\n<>|&()", *temp))
			temp++;
	}
	tokens[capacity->current_size].value = malloc(sizeof(char) * (temp - *input + 1));
	if (tokens[capacity->current_size].value == NULL)
		return ;
	ft_strlcpy(tokens[capacity->current_size].value, *input, temp - *input + 1);
	*input = temp;
}

t_tokens	*ft_tokenizer(char *input)
{
	t_capacity	capacity;
	t_tokens	*tokens;

	capacity.max_size = 10;
	capacity.current_size = 0;
	tokens = malloc(sizeof(t_tokens) * capacity.max_size);
	if (tokens == NULL)
		return (NULL);
	while (*input)
	{
		while (ft_strchr(" \t\n", *input) && *input)
			input++;
		if (capacity.max_size <= capacity.current_size)
		{
			return (NULL);
			//ft_realloc_when_tokens_full(&tokens, &capacity);
		}
		if (is_seperator(*input))
		{
			handle_word(&input, tokens, &capacity);
			//handle_seperator(&input, &tokens, &capacity);
		}
		else
			handle_word(&input, tokens, &capacity);
		capacity.current_size++;
	}
	print_tokens(tokens, capacity);
	return (tokens);
}

int    main(void)
{
    char    	*input;
	t_tokens	*tokens;

    //signal(SIGINT, handle_sigint);
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
        //printf("%s\n", input);
		tokens = ft_tokenizer(input);
		(void)tokens;
        free(input);
    }
    return (0);
}
