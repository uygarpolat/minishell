/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 11:17:35 by upolat            #+#    #+#             */
/*   Updated: 2024/11/11 15:15:17 by hpirkola         ###   ########.fr       */
/*   Updated: 2024/11/07 10:35:14 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../library/libft/libft.h"
#include "../includes/tokenizer.h"
#include "../includes/ast.h"

void	handle_sigint();
void	handle_sigquit();
void	print_ast(t_ast *node, int level);
void	init_signal();

int	execute_shell(char *input, int *code, char **new_envp)
{
	t_tokens	*tokens;
	t_capacity	capacity;
	t_ast		*ast;

	tokens = ft_tokenizer(input, &capacity, new_envp, *code);
	if (tokens)
	{
		ast = build_ast(tokens, 0, capacity.current_size - 1, 0);
		if (ast)
		{
			//print_ast(ast, 0);
			*code = execution(ast, &new_envp);
			free_ast(&ast);
		}
		free_tokens(tokens, &capacity);
	}
	else
		*code = 2;
	free_void((void **)&input, NULL);
	return (0);
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

int	preliminary_input_check(char **input, int *code)
{
	char	*temp;

	*input = readline("minishell> ");
	//if (*input == NULL)
	//	return (-2); // Commenting this back in kills the tester, why?
	temp = *input;
	while (ft_strchr(" \t\n", *temp) && *temp)
		temp++;
	if (!ft_strncmp(temp, "\0", 1))
	{
		free_void((void **)input, NULL);
		return (-2);
	}
	if (check_exit(temp) == -1)
	{
		*code = 0;
		free_void((void **)input, NULL);
		return (printf("exit\n"), -1);
	}
	if (*input)
		add_history(*input);
	return (0);
}

int	main(int argc, char **argv, char **envp)
{
	char	*input;
	char	**new_envp;
	int		code;
	int		input_res;

	((void)argc, (void)argv);
	new_envp = ft_strdup2(envp);
	if (!new_envp)
		return (1);
	code = 0;
	init_signal();
	signal(SIGINT, handle_sigint);
	signal(SIGQUIT, handle_sigquit);
	while (1)
	{
		input_res = preliminary_input_check(&input, &code);
		if (input_res == -2)
			continue ;
		else if (input_res == -1)
			break ;
		if (*input)
			add_history(input);
		tokens = ft_tokenizer(input, &capacity, new_envp, code);
		if (tokens)
		{
			ast = build_ast(tokens, 0, capacity.current_size - 1, 0);
			//if (ast)
				//print_ast(ast, 0);		
		}
		else
			continue ;
		if (ast)
			code = execution(ast, &new_envp);
		free_ast(&ast);
		free_tokens(tokens, &capacity);
		execute_shell(input, &code, new_envp);
	}
	free_2d_array((void ***)&new_envp);
	return (code);
}
