/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 15:00:15 by upolat            #+#    #+#             */
/*   Updated: 2024/10/14 15:34:59 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
	NUM_TYPES,
}	t_token_type;

typedef struct	s_tokens
{
	enum e_token_type	type;
	char				*value;
}	t_tokens;

typedef enum 	e_state
{
	DEFAULT_STATE,
	SINGLE_Q_STATE,
	DOUBLE_Q_STATE,
}	t_state;
