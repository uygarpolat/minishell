/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hpirkola <hpirkola@student.fi>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/15 15:37:30 by hpirkola          #+#    #+#             */
/*   Updated: 2024/10/15 16:35:08 by hpirkola         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H


typedef struct s_token
{
	enum tok_type	type;
	const char	*value;
}

typedef struct s_ast
{
	enum	type;
	struct	t_ast	**children;
	size_t		n_children;
	t_token		tok;
	int	code;
}	t_ast;

typedef struct	s_command
{
	char	*path; //the path
	char	**args; //the whole command, "ls -l"  tms.
}	t_command;

#endif
