/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 15:08:56 by hpirkola          #+#    #+#             */
/*   Updated: 2025/01/29 11:25:38 by hpirkola         ###   ########.fr       */
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
	int		*cmd_fd;
	int		fd_here[16];
	char	*heredocs[16];
}			t_put;

typedef struct s_minishell
{
	t_ast		*ast;
	t_pipes		p;
	t_tokens	*tokens;
	t_capacity	capacity;
	char		***envp;
	char		pwd[PATH_MAX];
	int			here;
	t_put		*cmd;
}				t_minishell;

typedef struct s_token_info
{
	t_tokens	*tokens;
	t_capacity	capacity;
	char		***envp;
}				t_token_info;

// execution.c
int			execution(t_ast *s, t_token_info *token_info,
				t_minishell *minishell);

// execution_utils.c
void		no_words(t_ast *s, t_minishell *minishell, t_put *cmd);
void		pipe_close(t_pipes *p, int pipe_no, int flag);
void		set_pwd(t_minishell *minishell, char **envp);
int			check_newlines(char **cmd);
void		unlink_here(t_put *cmd);

// execution_utils_2.c
void		pipe_fail(t_pipes *p);
void		free_pipes(t_pipes *p);
int			ft_pipe(t_pipes *p, int n, t_minishell *minishell, t_put *cmd);
char		**null_termination(char **new_envp, int flag, int j);
void		assign_out_fd(t_pipes *p, int n, int *out);

// execution_utils_3.c
void		put_heredoc_ctrl_d(char *str);
void		heredoc_echo(int fd, char *buf);

// init.c
int			initialize(t_put *cmd, t_minishell *minishell, \
				t_ast *s, t_token_info *token_info);
int			init_heredocs(t_put *cmd);

// path.c
char		*get_path(char **cmd, char **envp);

// exec_builtin.c
int			only_builtin(t_minishell *minishell, t_put *cmd);
void		run_builtin(t_ast *s, t_minishell *minishell, int n, t_put *cmd);
int			is_builtin(char **cmd);

// builtins.c
int			run_exit(t_ast *s, t_minishell *minishell,
				t_put *file);
int			run_cd(char **cmd, t_minishell *minishell);
int			run_echo(char **cmd);

// env_export.c
int			get_envp_size(char **envp);
int			run_export(char **cmd, t_minishell *minishell);
void		print_env(char **envp);

// env.c
char		**ch_envp(char **envp, char *cmd);
char		**add_env(char **envp, char *cmd);
char		**rm_envp(char **envp, char *cmd);

// files.c
void		dupping(t_minishell *minishell, t_pipes *p, t_put *cmd, int n);
int			get_in_out(t_ast *s, t_minishell *minishell, int n, int child);
int			check_in_out(t_ast *s, t_minishell *minishell, t_put *file, int n);

// pipes.c
void		close_pipes(t_minishell *minishell, int n);
int			count_pipes(t_ast *s);
int			mallocing(t_pipes *p);

// functions.c
int			waiting(int pid);
void		close_and_free(t_pipes *p, t_put *cmd, int n);
int			print_and_return(char *str, char *cmd, char *str2);
void		free_heredocs(t_put *cmd);

// ft_strdup2.c
char		**ft_strdup2(char **str);
char		**ft_strdup3(char **str, char *dir);

// errors.c
void		error(t_minishell *minishell, t_put *cmd);
void		error2(t_minishell *minishell, char *str, t_put *cmd);
void		print_and_exit(char *cmd, char *str, int code,
				t_minishell *minishell);
void		file_error(t_minishell *minishell, t_put *cmd);
int			print_error(char *str);

// error_checks.c
void		error_check(char *path, t_ast *s,
				t_minishell *minishell, t_put *cmd);
void		exit_heredocs(t_put *p);

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

// populate_command_node_utils.c
int			get_size_split(t_tokens *tokens, char *str);
int			malloc_when_token_is_expanded(t_tokens *tokens,
				t_ast *root, int i);

// print_ast.c
void		print_ast(t_ast *node, int level, int flag);

// ft_atol.c
long long	ft_atol(const char *str);

// heredoc.c
void		check_here(t_minishell *minishell, t_put *cmd);
void		here_loop(t_minishell *minishell, t_ast *ast, t_put *cmd, int *i);
int			here(t_tokens *token, t_ast *ast, t_put *cmd, int *i);

// var.c
char		*get_var(char **envp, char *key);
int			var_exists(char **envp, char *str);
char		**ch_var(char **envp, char *str);

#endif
