/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 15:00:15 by upolat            #+#    #+#             */
/*   Updated: 2024/10/26 19:00:30 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOKENIZER_H
# define TOKENIZER_H

# include <stdio.h>
# include <stdlib.h>
# include <signal.h>
# include <unistd.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <readline/history.h>
# include <string.h>

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
}	t_tokens;

typedef struct	s_capacity
{
	int			max_size;
	int			current_size;
}	t_capacity;

t_tokens	*ft_tokenizer(char *input, t_capacity *capacity, char **envp);
void		free_tokens(t_tokens *tokens, t_capacity *capacity);

#endif
