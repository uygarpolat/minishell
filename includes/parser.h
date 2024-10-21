/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 11:40:05 by upolat            #+#    #+#             */
/*   Updated: 2024/10/21 11:28:34 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
# define PARSER_H

# include <stdio.h>
# include <stdlib.h>
# include <signal.h>
# include <unistd.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <readline/history.h>
# include <string.h>
# include "tokenizer.h"

typedef enum	e_ast_node_type {
	AST_COMMAND,
	AST_PIPE,
	AST_REDIR_OUT,
	AST_REDIR_APPEND,
	AST_REDIR_IN,
	AST_HEREDOC,
	AST_AND,
	AST_OR
}	t_ast_node_type;

typedef struct	s_ast_node
{
	t_ast_node_type		type;
	t_tokens 			*token;
	struct s_ast_node	*left;
	struct s_ast_node	*right;
	struct s_ast_node	*redir_target;
}	t_ast_node;

t_ast_node *build_ast(t_tokens *tokens, int start, int end);

#endif
