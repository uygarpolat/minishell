/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hpirkola <hpirkola@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 15:08:56 by hpirkola          #+#    #+#             */
/*   Updated: 2024/10/23 14:40:12 by hpirkola         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef AST_H
# define AST_H

# include <stdio.h>
# include <stdlib.h>
# include <signal.h>
# include <unistd.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <readline/history.h>
# include <string.h>
# include <sys/wait.h>
# include "libft/libft.h"
//# include "tokenizer.h"

typedef enum    e_token_type
{
    TOKEN_WORD,
    TOKEN_REDIR_IN,
    TOKEN_REDIR_OUT,
    TOKEN_APPEND,
    TOKEN_HEREDOC,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_OPEN_PAREN,
    TOKEN_CLOSE_PAREN,
    TOKEN_PIPE,
    TOKEN_UNKNOWN,
    NUM_TYPES,
}    t_token_type;

typedef struct    s_tokens
{
    enum e_token_type    type;
    char                *value;
}    t_tokens;

typedef enum    e_ast_node_type {
    AST_COMMAND,
    AST_PIPE,
    AST_REDIR_OUT,
    AST_REDIR_APPEND,
    AST_REDIR_IN,
    AST_HEREDOC,
    AST_AND,
    AST_OR
}    t_ast_node_type;

typedef struct    s_ast
{
	t_tokens	token;
	struct s_ast    *left;
	struct s_ast    *right;
	struct s_ast    *redir_target;
	int	code;
}    t_ast;

typedef struct s_pipe
{
	int	*pipes;
	int	*pids;
	int	count;
	int	i;
	int	o;
}	t_pipes;

typedef struct s_command
{
	char **args;
	char	*path;
}	t_command;
//execution.c
int	execution(t_ast *s, char **envp);

//path.c
char	*get_path(char **cmd, char **envp);
char	**paths(char **envp);

#endif
