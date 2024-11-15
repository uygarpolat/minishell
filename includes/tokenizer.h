/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 15:00:15 by upolat            #+#    #+#             */
/*   Updated: 2024/11/15 02:59:23 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOKENIZER_H
# define TOKENIZER_H

# include <stdio.h>
# include <stdlib.h>
# include <signal.h>
# include <unistd.h>
# include <dirent.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <readline/history.h>
# include <string.h>
# include <errno.h>

extern int	g_signal;

typedef enum	e_token_type
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
}	t_token_type;

typedef struct	s_tokens
{
	enum e_token_type	type;
	char				*value;
	char				**globbed;
	int					*code;
}						t_tokens;

typedef struct	s_capacity
{
	int			max_size;
	int			current_size;
}				t_capacity;

typedef struct	s_quote
{
	int			single_q_count;
	int			double_q_count;
	char		*single_first_unclosed;
	char		*double_first_unclosed;
	char		*str_initial;
}				t_quote;

typedef struct	s_globber
{
	DIR				*dir;
	struct dirent	*entry;
	int				loc;
	int				flag;
}					t_globber;

typedef struct p_arrays
{
	int		*int_array_new;
	int		*int_array_old;
	int		*int_array_new_start;
	int		*int_array_old_start;
	char	**envp;
	int		*code;
}			t_arrays;

t_tokens	*ft_tokenizer(char *input, t_capacity *capacity, char **envp, int *code);
void		*free_tokens(t_tokens *tokens, t_capacity *capacity);
char		*ft_strjoin_free(char *s1, char *s2);
int			identify_token(t_token_type type);
char		*back_to_char(t_tokens *tokens, int *int_array);
void		error_handler(char *cause_str, char *error_str, int *code_address, int exit_code);
int			find_matching_paren(t_tokens *tokens, int start, int end);
int			*ultimate_dollar_expansion(t_arrays *a, t_token_type type, int flag, int len);

#endif
