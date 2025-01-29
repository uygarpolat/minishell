/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 11:17:35 by upolat            #+#    #+#             */
/*   Updated: 2025/01/29 16:48:59 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../library/libft/libft.h"
#include "../includes/tokenizer.h"
#include "../includes/ast.h"
#include "../includes/signals.h"

int	g_signal = 0;

void	format_tokens(t_tokens **tokens, t_capacity *capacity)
{
	int	read_i;
	int	write_i;

	write_i = 0;
	read_i = 0;
	while (read_i < capacity->current_size)
	{
		if ((*tokens)[read_i].value != NULL)
		{
			(*tokens)[write_i] = (*tokens)[read_i];
			write_i++;
		}
		read_i++;
	}
	capacity->current_size = write_i;
	while (write_i < capacity->max_size)
	{
		(*tokens)[write_i].type = TOKEN_UNKNOWN;
		(*tokens)[write_i].value = NULL;
		(*tokens)[write_i].globbed = NULL;
		(*tokens)[write_i].code = NULL;
		write_i++;
	}
	if (!capacity->current_size)
		free_void((void **)tokens, NULL);
}

int	execute_shell(char *input, int *code,
		char ***new_envp, t_minishell *minishell)
{
	t_tokens		*tokens;
	t_capacity		capacity;
	t_ast			*ast;
	t_token_info	token_info;

	tokens = ft_tokenizer(input, &capacity, *new_envp, code);
	if (tokens)
		format_tokens(&tokens, &capacity);
	if (tokens)
	{
		ast = build_ast(tokens, 0, capacity.current_size - 1, 0);
		if (ast)
		{
			print_ast(ast, 0, 0);
			g_signal = 0;
			token_info.tokens = tokens;
			token_info.capacity = capacity;
			token_info.envp = new_envp;
			*code = execution(ast, &token_info, minishell);
			free_ast(&ast);
		}
		free_tokens(&tokens, &capacity);
	}
	free_void((void **)&input, NULL);
	return (*code);
}

int	check_exit(char *temp)
{
	if (!ft_strncmp(temp, "exit", 4))
	{
		temp += 4;
		if (*temp == '\0')
			return (-1);
		else
		{
			while (ft_strchr(" \t\n", *temp) && *temp)
				temp++;
			if (*temp == '\0')
				return (-1);
		}
	}
	return (0);
}

int	preliminary_input_check(char **input)
{
	char	*temp;

	*input = readline("minishell> ");
	if (*input == NULL)
		return (-1);
	temp = *input;
	while (ft_strchr(" \t\n", *temp) && *temp)
		temp++;
	if (!ft_strncmp(temp, "\0", 1))
		return (free_void((void **)input, NULL), -2);
	if (check_exit(temp) == -1)
	{
		free_void((void **)input, NULL);
		return (printf("exit\n"), -1);
	}
	if (**input)
		add_history(*input);
	return (0);
}

int	main(int argc, char **argv, char **envp)
{
	char		*input;
	char		**new_envp;
	int			code;
	int			input_res;
	t_minishell	minishell;

	code = 0;
	new_envp = ft_strdup2(envp);
	if (!new_envp)
		return (error_handler(NULL, NULL, &code, 1), code);
	set_pwd(&minishell, new_envp);
	if (init_term_and_signal(argc, argv, &code))
		return (free_2d_array((void ***)&new_envp), code);
	while (1)
	{
		input_res = preliminary_input_check(&input);
		if (input_res == -2)
			continue ;
		else if (input_res == -1)
			break ;
		code = execute_shell(input, &code, &new_envp, &minishell);
	}
	free_2d_array((void ***)&new_envp);
	return (code);
}
