/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 11:17:35 by upolat            #+#    #+#             */
/*   Updated: 2024/10/27 21:18:36 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../library/libft/libft.h"
#include "../includes/tokenizer.h"
#include "../includes/ast.h"

void	handle_sigint();
void	handle_sigquit();
void	print_ast(t_ast *node, int level);

int	main(int argc, char **argv, char **envp)
{
	char		*input;
	t_tokens	*tokens;
	t_capacity	capacity;
	t_ast		*ast;

	(void) argc;
	(void) argv;
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
		tokens = ft_tokenizer(input, &capacity, envp);
		if (tokens)
		{
			ast = build_ast(tokens, 0, capacity.current_size - 1);
			print_ast(ast, 0);
		}
		//execution(ast, envp);
		free_ast(ast);
		free_tokens(tokens, &capacity);
		free(input);
	}
	return (0);
}

