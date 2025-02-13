/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 15:00:15 by upolat            #+#    #+#             */
/*   Updated: 2025/01/27 21:57:06 by upolat           ###   ########.fr       */
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
# include "../library/libft/libft.h"

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
	int					flag;
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

// tokenizer.c
t_tokens	*ft_tokenizer(char *input, t_capacity *capacity,
				char **envp, int *code);

// error_and_grammar.c
int			tokens_error_checker(t_tokens *tokens,
				t_capacity *capacity, int i, int k);
char		*check_match_count(t_tokens *tokens,
				int *int_array, int match_count);

// expand_dollar.c
int			*ultimate_dollar_expansion(t_arrays *a, t_token_type type,
				int flag, int len);

// expand_dollar_2.c
int			str_combined(t_arrays *a, t_token_type type, int *len, int flag);

// globbing.c
char		*expand_wildcard(int *int_array, t_tokens *tokens,
				int loc, int flag);

// malloc_and_free.c
void		free_tokens(t_tokens **tokens, t_capacity *capacity);
t_tokens	*realloc_tokens_when_full(t_tokens *tokens,
				t_capacity *capacity, int i);
int			check_char_in_intarray(int *int_array, char chr);

// tokenization_utils.c
int			is_seperator(char c, char c_plus_one);
void		assign_token_types(char **temp, t_token_type *type,
				t_token_type type1, t_token_type type2);
int			encode_char_with_flag(int c, int shift_amount);
char		*back_to_char(t_tokens *tokens, int *int_array);
void		error_handler(char *cause_str, char *error_str,
				int *code_address, int exit_code);

// expansion_and_wildcard.c
int			handle_expansion_and_wildcard(t_tokens *tokens,
				t_capacity *capacity, char **envp);

// handle_word_and_separator.c
int			handle_seperator(char **input,
				t_tokens *tokens, t_capacity *capacit, int *code);
int			handle_word(char **input, t_tokens *tokens, t_capacity *capacity,
				int *code);

// populate_tokens.c
int			populate_tokens(char *str, int *code, int *int_array, int m);

// print_tokens.c
void		print_tokens(t_tokens *tokens, t_capacity *capacity, int flag);

#endif
