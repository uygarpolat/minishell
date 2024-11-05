/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 11:17:35 by upolat            #+#    #+#             */
/*   Updated: 2024/11/05 01:54:49 by upolat           ###   ########.fr       */
/*   Updated: 2024/10/28 17:20:14 by hpirkola         ###   ########.fr       */
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

	(void) argc;
	(void) argv;
	new_envp = ft_strdup2(envp);
	if (!new_envp)
		return (1);
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
		input = readline("minishell> ");
		if (!input)
			break ;
		if (!ft_strncmp(input, "exit", 5))
			break ;
		if (*input)
			add_history(input);
		tokens = ft_tokenizer(input, &capacity, new_envp);
		if (tokens)
		{
			ast = build_ast(tokens, 0, capacity.current_size - 1, 0);
			if (ast)
				print_ast(ast, 0);		
		}
		else
			continue ;
		//execution(ast, new_envp);
		free_ast(&ast);
		free_tokens(tokens, &capacity);
		free(input);
	}
	free_2d_array((void ***)&new_envp);
	return (0);
}
