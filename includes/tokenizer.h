/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 15:00:15 by upolat            #+#    #+#             */
/*   Updated: 2024/11/16 02:17:44 by upolat           ###   ########.fr       */
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
# include <string.h>
# include <errno.h>
# include <unistd.h>

typedef enum e_token_type
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

typedef struct s_tokens
{
	enum e_token_type	type;
	char				*value;
	char				**globbed;
	int					*code;
}						t_tokens;

typedef struct s_capacity
{
	int			max_size;
	int			current_size;
}				t_capacity;

typedef struct s_quote
{
	int			single_q_count;
	int			double_q_count;
	char		*single_first_unclosed;
	char		*double_first_unclosed;
	char		*str_initial;
}				t_quote;

typedef struct s_globber
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

t_tokens	*ft_tokenizer(char *input, t_capacity *capacity,
				char **envp, int *code);
void		*free_tokens(t_tokens *tokens, t_capacity *capacity);
int			identify_token(t_token_type type);
char		*back_to_char(t_tokens *tokens, int *int_array);
void		error_handler(char *cause_str, char *error_str,
				int *code_address, int exit_code);
int			find_matching_paren(t_tokens *tokens, int start, int end);
int			*ultimate_dollar_expansion(t_arrays *a, t_token_type type,
				int flag, int len);
t_tokens	*realloc_tokens_when_full(t_tokens *tokens,
				t_capacity *capacity, int i);
int			is_seperator(char c, char c_plus_one);
int			handle_seperator(char **input, t_tokens *tokens,
				t_capacity *capacity);
int			handle_word(char **input, t_tokens *tokens, t_capacity *capacity);
void		print_tokens(t_tokens *tokens, t_capacity *capacity);
int			tokens_error_checker(t_tokens *tokens, t_capacity *capacity);
int			handle_expansion_and_wildcard(t_tokens *tokens,
				t_capacity *capacity, char **envp);
void		syntax_error_near(t_tokens *tokens, int loc);
int			populate_tokens(char *str, int *code, int *int_array, int m);
char		*expand_wildcard(int *int_array, t_tokens *tokens,
				int loc, int flag);
void		assign_token_types(char **temp, t_token_type *type,
				t_token_type type1, t_token_type type2);
int			str_combined(t_arrays *a, t_token_type type, int *len, int flag);
int			encode_char_with_flag(int c, int shift_amount);
void		print_tokens(t_tokens *tokens, t_capacity *capacity);

#endif
