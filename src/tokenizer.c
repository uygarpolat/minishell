/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 11:16:11 by upolat            #+#    #+#             */
/*   Updated: 2024/10/17 10:32:51 by upolat           ###   ########.fr       */
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
/*
void	print_tokens(t_tokens *tokens, t_capacity capacity)
{
	int	i;

	i = -1;
	while (++i < capacity.current_size)
		printf("token %d is: %s\n", i, tokens[i].value);
}
*/
void	print_tokens(t_tokens *tokens, t_capacity capacity)
{
	char *token_type_str[] = {
		"TOKEN_WORD",
		"TOKEN_REDIR_IN",
		"TOKEN_REDIR_OUT",
		"TOKEN_APPEND",
		"TOKEN_HEREDOC",
		"TOKEN_AND",
		"TOKEN_OR",
		"TOKEN_OPEN_PAREN",
		"TOKEN_CLOSE_PAREN",
		"TOKEN_PIPE",
		"TOKEN_UNKNOWN",
		"NUM_TYPES"
	};

	for (int i = 0; i < capacity.current_size; i++)
		printf("token %d: %s –---- type: %s\n", i, tokens[i].value, token_type_str[tokens[i].type]);
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


void	handle_seperator(char **input, t_tokens *tokens, t_capacity *capacity)
{
	char *temp;
	t_token_type type;

	temp = *input;
	type = TOKEN_UNKNOWN;


	if (*temp == '<')
	{
		temp++;
		if (*temp == '<')
		{
			temp++;
			type = TOKEN_HEREDOC;
		}
		else
			type = TOKEN_REDIR_IN;
	}
	else if (*temp == '>')
	{
		temp++;
		if (*temp == '>')
		{
			temp++;
			type = TOKEN_APPEND;
		}
		else
			type = TOKEN_REDIR_OUT;
	}
	else if (*temp == '&' && *(temp + 1) == '&')
	{
		temp += 2;
		type = TOKEN_AND;
	}
	else if (*temp == '|')
	{
		temp++;
		if (*temp == '|')
		{
			temp++;
			type = TOKEN_OR;
		}
		else
			type = TOKEN_PIPE;
	}
	else if (*temp == '(')
	{
		temp++;
		type = TOKEN_OPEN_PAREN;
	}
	else if (*temp == ')')
	{
		temp++;
		type = TOKEN_CLOSE_PAREN;
	}
	else if (*temp == '&')
	{
		temp++;
		type = TOKEN_UNKNOWN;
	}
	else
	{
		temp++;
		type = TOKEN_UNKNOWN;
	}
	tokens[capacity->current_size].value = malloc(sizeof(char) * (temp - *input + 1));
	if (!tokens[capacity->current_size].value)
		return ;
	ft_strlcpy(tokens[capacity->current_size].value, *input, temp - *input + 1);
	tokens[capacity->current_size].type = type;
	*input = temp;
}

void	handle_word(char **input, t_tokens *tokens, t_capacity *capacity)
{
	char	*temp;

	temp = *input;
	if (*temp == '\'')
	{
		++temp;
		while (*temp && ft_strncmp(temp, "'", 1))
			temp++;
		++temp;
	}
	else if (*temp == '"')
	{
		++temp;
		while (*temp && ft_strncmp(temp, "\"", 1))
			temp++;
		++temp;
	}
	else
	{
		//++temp;
		while (*temp && !ft_strchr(" \n\t<>|&()\"'", *temp))
			temp++;
	}
	tokens[capacity->current_size].value = malloc(sizeof(char) * (temp - *input + 1));
	if (tokens[capacity->current_size].value == NULL)
		return ;
	ft_strlcpy(tokens[capacity->current_size].value, *input, temp - *input + 1);
	tokens[capacity->current_size].type = TOKEN_WORD;
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
			handle_seperator(&input, tokens, &capacity);
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
