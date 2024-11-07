/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 11:17:35 by upolat            #+#    #+#             */
/*   Updated: 2024/11/08 01:05:21 by upolat           ###   ########.fr       */
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

int	main(int argc, char **argv, char **envp)
{
	char		*input;
	t_tokens	*tokens;
	t_capacity	capacity;
	t_ast	*ast;
	char	**new_envp;
	int	code;

	(void) argc;
	(void) argv;
	input = NULL;
	new_envp = NULL;
	new_envp = ft_strdup2(envp);
	if (!new_envp)
		return (1);
	code = 0;
	/*while (new_envp)
	{
		printf("%s\n", *new_envp);
		new_envp++;
	}*/
	init_signal();
	signal(SIGINT, handle_sigint);
	signal(SIGQUIT, handle_sigquit);
	while (1)
	{
		free_void((void **)&input, NULL);
		input = readline("minishell> ");
		if (!input)
			break ;
		if (!ft_strncmp(input, "\0", 1))
			continue ;
		if (!ft_strncmp(input, "exit", 5))
			break ;
		if (*input)
			add_history(input);
		tokens = ft_tokenizer(input, &capacity, new_envp, code);
		if (tokens)
		{
			ast = build_ast(tokens, 0, capacity.current_size - 1, 0);
			if (ast)
			{
				//print_ast(ast, 0);
				//code = execution(ast, &new_envp);
				free_ast(&ast);
			}
			else
				code = 2;
			free_tokens(tokens, &capacity);
		}
		else
		{
			code = 2;
			continue ;
		}
		//if (ast)
		//	code = execution(ast, &new_envp);
		//free_ast(&ast);
		//free_tokens(tokens, &capacity);
	}
	free_void((void **)&input, NULL);
	free_2d_array((void ***)&new_envp);
	return (code);
}
