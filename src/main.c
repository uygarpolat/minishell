/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 11:17:35 by upolat            #+#    #+#             */
/*   Updated: 2025/01/15 14:24:56 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../library/libft/libft.h"
#include "../includes/tokenizer.h"
#include "../includes/ast.h"
#include "../includes/signals.h"

void	display_welcome_message(int *code, char **new_envp, int flag);

int	g_signal = 0;

int	check_before_execute(t_tokens *tokens, t_capacity capacity, char *input, int *code)
{
	// char	**split;
	
	// (void)capacity;
	// split = ft_split(input, ' ');
	
	// if (tokens && tokens[0].value[0] == '\0' && !ft_strchr(split[0], '$'))
	// {
	// 	error_handler(&tokens[0].value[0], "command not found", code, 127);
	// 	free_2d_array((void ***)&split);
	// 	return (1);
	// }
	// free_2d_array((void ***)&split);
	// return (0);

	(void)code;
	if (tokens && capacity.current_size == 1 && tokens[0].value[0] == '\0' && ft_strchr(input, '$'))
		return (1);
	return(0);
}

int	execute_shell(char *input, int *code, char ***new_envp)
{
	t_tokens	*tokens;
	t_capacity	capacity;
	t_ast		*ast;

	tokens = ft_tokenizer(input, &capacity, *new_envp, code);
	if (check_before_execute(tokens, capacity, input, code))
		free_tokens(tokens, &capacity);
	else if (tokens)
	{
		ast = build_ast(tokens, 0, capacity.current_size - 1, 0);
		if (ast)
		{
			print_ast(ast, 0, 0);
			g_signal = 0;
			*code = execution(ast, new_envp, tokens, capacity);
			free_ast(&ast);
		}
		free_tokens(tokens, &capacity);
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
	char	*input;
	char	**new_envp;
	int		code;
	int		input_res;

	code = 0;
	new_envp = ft_strdup2(envp);
	if (!new_envp)
		return (error_handler(NULL, NULL, &code, 1), code);
	if (init_term_and_signal(argc, argv, &code))
		return (free_2d_array((void ***)&new_envp), EXIT_FAILURE);
	display_welcome_message(&code, new_envp, 0);
	while (1)
	{
		input_res = preliminary_input_check(&input);
		if (input_res == -2)
			continue ;
		else if (input_res == -1)
			break ;
		code = execute_shell(input, &code, &new_envp);
	}
	free_2d_array((void ***)&new_envp);
	return (code);
}
