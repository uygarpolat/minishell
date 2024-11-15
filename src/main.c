/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 11:17:35 by upolat            #+#    #+#             */
/*   Updated: 2024/11/15 02:51:33 by upolat           ###   ########.fr       */
/*   Updated: 2024/11/11 15:15:17 by hpirkola         ###   ########.fr       */
/*   Updated: 2024/11/07 10:35:14 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../library/libft/libft.h"
#include "../includes/tokenizer.h"
#include "../includes/ast.h"

void	handle_sigint(int signum);
void	handle_sigquit(int signum);
void	print_ast(t_ast *node, int level);
int		init_signal(int argc, char **argv);

int	g_signal = 0;

int	execute_shell(char *input, int *code, char **new_envp)
{
	t_tokens	*tokens;
	t_capacity	capacity;
	t_ast		*ast;

	tokens = ft_tokenizer(input, &capacity, new_envp, code);
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
	//else
	//	*code = 2;
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

int	preliminary_input_check(char **input, int *code)
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
		*code = 0;
		free_void((void **)input, NULL);
		return (printf("exit\n"), -1);
	}
	if (**input)
		add_history(*input);
	return (0);
}

void	display_welcome_message(int *code, char **new_envp)
{
	char	*payload;

	payload = "echo \"\033[1;34m+--------------------\
--+\033[0m\n\033[1;34m| \033[1;37mWelcome to Minishell\
\033[0m\033[1;34m |\033[0m\n\033[1;34m+----------------------+\033[0m\"";
	*code = execute_shell(ft_strdup("clear"), code, new_envp);
	*code = execute_shell(ft_strdup(payload), code, new_envp);
}

int	main(int argc, char **argv, char **envp)
{
	char	*input;
	char	**new_envp;
	int		code;
	int		input_res;

	new_envp = ft_strdup2(envp);
	if (!new_envp)
		return (1);
	code = 0;
	if (init_signal(argc, argv))
		return (code);
	//display_welcome_message(&code, new_envp);
	while (1)
	{
		input_res = preliminary_input_check(&input, &code);
		if (input_res == -2)
			continue ;
		else if (input_res == -1)
			break ;
		if (*input)
			add_history(input);
		code = execute_shell(input, &code, new_envp);
	}
	free_2d_array((void ***)&new_envp);
	return (code);
}
