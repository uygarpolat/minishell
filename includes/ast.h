/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hpirkola <hpirkola@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 15:08:56 by hpirkola          #+#    #+#             */
/*   Updated: 2024/11/28 12:03:07 by hpirkola         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef AST_H
# define AST_H

# include <stdio.h>
# include <stdlib.h>
# include <signal.h>
# include <unistd.h>
# include <fcntl.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <readline/history.h> // Remove this?
# include <string.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <limits.h>
# include <errno.h>
# include "../library/libft/libft.h"
# include "tokenizer.h"

typedef enum e_ast_node_type
{
	AST_COMMAND,
	AST_PIPE,
	AST_REDIR_OUT,
	AST_REDIR_APPEND,
	AST_REDIR_IN,
	AST_HEREDOC,
	AST_AND,
	AST_OR
}	t_ast_node_type;

typedef struct s_ast
{
	t_ast_node_type	type;
	t_tokens		*token;
	char			**words;
	struct s_ast	*left;
	struct s_ast	*right;
	struct s_ast	*redir_target;
	int				code;
	int				*code_parser;
}					t_ast;

typedef struct s_precedence
{
	int	i;
	int	start;
	int	end;
	int	prec;
	int	lowest_prec;
	int	lowest_prec_pos;
}		t_precedence;

typedef struct s_pipe
{
	int	**pipes;
	int	*pids;
	int	count;
	int	o_count;
	int	i;
	int	o;
}		t_pipes;

typedef struct s_put
{
	char	*infile;
	char	*outfile;
	char	o_type;
	int		in;
	int		out;
	int		stdin2;
	int		stdout2;
}			t_put;

typedef struct s_minishell
{
	t_ast	*ast;
	t_pipes	p;
	char	pwd[PATH_MAX];
}			t_minishell;

//execution.c
int		execution(t_ast *s, char ***envp);
void	close_and_free(t_pipes *p, t_put *cmd);
void	get_in_out(t_ast *s, t_put *cmd, t_minishell *minishell);
int	open_files(t_put *cmd);
void	dupping(t_minishell *minishell, t_pipes *p, t_put *cmd, int n);

//path.c
char	*get_path(char **cmd, char **envp);
char	**paths(char **envp);

//builtins.c
int	execute_builtin(t_ast *s, char **cmd, char ***envp, t_minishell *minishell, int n, t_put *file);
int	is_builtin(char **cmd);

// ft_strdup2.c
char		**ft_strdup2(char **str);
char		**ft_strdup3(char **str, char *dir);

//errors.c
void	error(t_minishell *minishell, t_put *cmd);
void	error2(t_minishell *minishell, char *str, t_put *cmd);

// parser.c
t_ast		*build_ast(t_tokens *tokens, int start, int end, int code);
int			redirection_node_creator(t_tokens *tokens, t_ast *root, int *i);

// parser_utils.c
void		assign_token_type(t_ast *node, t_tokens *token);
int			establish_lowest_precedence(t_tokens *tokens, t_precedence *p);
int			find_matching_paren(t_tokens *tokens, int start, int end);
int			identify_token(t_token_type type);

// memory_and_error.c
void		free_ast(t_ast **node);
t_tokens	*copy_token(t_tokens *token);
int			cleanup_populate_command_node(t_ast **root, int *error_code);
void		syntax_error_near(t_tokens *tokens, int loc);

// populate_command_node.c
int			populate_command_node(t_tokens *tokens,
				t_ast *root, int start, int *end);

// print_ast.c
void		print_ast(t_ast *node, int level, int flag);

//ft_atol.c
int	ft_atol(const char *str);
#endif
