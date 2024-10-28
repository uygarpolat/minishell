/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hpirkola <hpirkola@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 15:08:56 by hpirkola          #+#    #+#             */
/*   Updated: 2024/10/28 16:40:46 by hpirkola         ###   ########.fr       */
/*   Updated: 2024/10/24 14:52:52 by hpirkola         ###   ########.fr       */
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
# include <limits.h>
# include "../library/libft/libft.h"
# include "tokenizer.h"

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
	t_ast_node_type	type;
	t_tokens	*token;
	struct s_ast    *left;
	struct s_ast    *right;
	struct s_ast    *redir_target;
	int	code;
}    t_ast;

typedef struct s_pipe
{
	int	**pipes;
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

typedef struct s_minishell
{
	t_ast	*ast;
	t_pipes	p;
	char	*pwd;
}	t_minishell;

t_ast	*build_ast(t_tokens *tokens, int start, int end);
void	free_ast(t_ast *node);

//execution.c
int	execution(t_ast *s, char **envp);

//path.c
char	*get_path(char **cmd, char **envp);
char	**paths(char **envp);

//builtins.c
int	execute_builtin(char **cmd, char **envp, t_minishell *minishell);
int	is_builtin(t_tokens *token);

//ft_strdup2.c
char	**ft_strdup2(char **str);
char	**ft_strdup3(char **str, char *dir);
#endif
