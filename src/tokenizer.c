/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 11:16:11 by upolat            #+#    #+#             */
/*   Updated: 2024/10/24 14:26:09 by upolat           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../library/libft/libft.h"
#include "../includes/tokenizer.h"
#include "../includes/ast.h"

void	handle_sigint()
{
	write(STDOUT_FILENO, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

void	handle_sigquit()
{
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

void	print_tokens(t_tokens *tokens, t_capacity *capacity)
{
	int		i;
	char	formatted_token[100];
	char	*token_type_str[] = {
		"TOKEN_WORD",
		"TOKEN_REDIR_IN",
		"TOKEN_REDIR_OUT",
		"TOKEN_APPEND",
		"TOKEN_HEREDOC",
		"TOKEN_AND",
		"TOKEN_OR",
		"TOKEN_OPEN_PAREN",
		"TOKEN_CLOSE_PAREN",
		"TOKEN_PIPE",
		"TOKEN_UNKNOWN",
		"NUM_TYPES"
	};
	i = -1;
	while (++i < capacity->current_size)
	{
		snprintf(formatted_token, sizeof(formatted_token), "token %d: %s", i, tokens[i].value);
		printf("%-30s type: %s\n", formatted_token, token_type_str[tokens[i].type]);
	}
}

void	print_ast(t_ast *node, int level)
{
	int	indent_level;

	if (node == NULL)
		return ;
	indent_level = -1;
	while (++indent_level < level)
		printf("  ");
	if (node->type == AST_COMMAND)
		printf("AST_COMMAND [TOKEN: %s]\n", node->token->value);
	else if (node->type == AST_PIPE)
		printf("AST_PIPE [TOKEN: |]\n");
	else if (node->type == AST_REDIR_OUT)
		printf("AST_REDIR_OUT [TOKEN: >]\n");
	else if (node->type == AST_REDIR_APPEND)
		printf("AST_REDIR_APPEND [TOKEN: >>]\n");
	else if (node->type == AST_REDIR_IN)
		printf("AST_REDIR_IN [TOKEN: <]\n");
	else if (node->type == AST_HEREDOC)
		printf("AST_HEREDOC [TOKEN: <<]\n");
	else if (node->type == AST_AND)
		printf("AST_AND [TOKEN: &&]\n");
	else if (node->type == AST_OR)
		printf("AST_OR [TOKEN: ||]\n");
	else
		printf("Unknown node type\n");
	if (node->left != NULL)
		print_ast(node->left, level + 1);
	if (node->right != NULL)
		print_ast(node->right, level + 1);
	indent_level = -1;
	t_ast	*redir_node = node->redir_target;
	while (redir_node != NULL)
	{
		indent_level = -1;
		while (++indent_level < level + 1)
			printf("  ");
		char	*redir_symbol;
		if (redir_node->type == AST_REDIR_OUT)
			redir_symbol = ">";
		else if (redir_node->type == AST_REDIR_APPEND)
			redir_symbol = ">>";
		else if (redir_node->type == AST_REDIR_IN)
			redir_symbol = "<";
		else if (redir_node->type == AST_HEREDOC)
			redir_symbol = "<<";
		else
			redir_symbol = "?";
		printf("REDIR: %s [TOKEN: %s]\n", redir_symbol, redir_node->token->value);
		redir_node = redir_node->redir_target;
	}
}

int	is_space(char c)
{
	if (c == ' ')
		return (1);
	return (0);
}

int	is_seperator(char c)
{
	if (c == '<' || c == '>' || c == '&' || c == '|'
		|| c == '(' || c == ')')
		return (1);
	return (0);
}

void	free_tokens(t_tokens *tokens, t_capacity *capacity)
{
	int	i;

	i = 0;
	while (i < capacity->max_size)
	{
		if (i < capacity->current_size)
			free(tokens[i].value);
		tokens[i].value = NULL;
		i++;
	}
	free(tokens);
	tokens = NULL;
}

t_tokens	*ft_realloc_tokens_when_full(t_tokens *tokens, t_capacity *capacity)
{
	int			i;
	t_tokens	*new_tokens;

	new_tokens = malloc(sizeof(t_tokens) * (capacity->max_size * 2));
	if (new_tokens == NULL)
	{
		free_tokens(tokens, capacity);
		return (NULL);
	}
	i = -1;
	while (++i < capacity->current_size)
	{
		new_tokens[i].value = ft_strdup(tokens[i].value);
		new_tokens[i].type = tokens[i].type;
	}
	free_tokens(tokens, capacity);
	capacity->max_size *= 2;
	return (new_tokens);
}

int	malloc_individual_tokens(t_tokens *tokens, char **input, char *temp, t_capacity *capacity, t_token_type type)
{
	tokens[capacity->current_size].value = malloc(sizeof(char) * (temp - *input + 1));
	if (tokens[capacity->current_size].value == NULL)
		return (1);
	ft_strlcpy(tokens[capacity->current_size].value, *input, temp - *input + 1);
	tokens[capacity->current_size].type = type;
	capacity->current_size++;
	*input = temp;
	return (0);
}

void	handle_seperator(char **input, t_tokens *tokens, t_capacity *capacity)
{
	char			*temp;
	t_token_type	type;

	temp = *input;
	type = TOKEN_UNKNOWN;
	if (*temp == '<')
	{
		temp++;
		if (*temp == '<')
		{
			temp++;
			type = TOKEN_HEREDOC;
		}
		else
			type = TOKEN_REDIR_IN;
	}
	else if (*temp == '>')
	{
		temp++;
		if (*temp == '>')
		{
			temp++;
			type = TOKEN_APPEND;
		}
		else
			type = TOKEN_REDIR_OUT;
	}
	else if (*temp == '&' && *(temp + 1) == '&')
	{
		temp += 2;
		type = TOKEN_AND;
	}
	else if (*temp == '|')
	{
		temp++;
		if (*temp == '|')
		{
			temp++;
			type = TOKEN_OR;
		}
		else
			type = TOKEN_PIPE;
	}
	else if (*temp == '(')
	{
		temp++;
		type = TOKEN_OPEN_PAREN;
	}
	else if (*temp == ')')
	{
		temp++;
		type = TOKEN_CLOSE_PAREN;
	}
	else if (*temp == '&')
	{
		temp++;
		type = TOKEN_UNKNOWN;
	}
	else
	{
		temp++;
		type = TOKEN_UNKNOWN;
	}
	if (malloc_individual_tokens(tokens, input, temp, capacity, type))
		return ;
}

char	*skip_a_char(char *str, char c)
{
	str++;
	while (*str && *str != c)
		str++;
	if (*str)
		str++;
	return (str);
}

int	skip_quotes_and_ampersand(char **temp)
{
	if (ft_strchr(" \n\t<>|&()\"'", **temp))
	{
		if (**temp == '&')
		{
			if (**(temp + 1) == '&')
				return (0);
			else
				return (1);
		}
		else if (**temp == '"')
		{
			*temp = skip_a_char(*temp, '"');
			return (0);
		}
		else if (**temp == '\'')
		{
			*temp = skip_a_char(*temp, '\'');
			return (0);
		}
		else
			return (0);
	}
	return (1);
}

void	handle_word(char **input, t_tokens *tokens, t_capacity *capacity)
{
	char	*temp;

	temp = *input;
	if (!*temp)
		return ;
	if (*temp == '\'')
		temp = skip_a_char(temp, '\'');
	else if (*temp == '"')
		temp = skip_a_char(temp, '"');
	else
	{
		while (*temp && skip_quotes_and_ampersand(&temp))
			temp++;
	}
	if (malloc_individual_tokens(tokens, input, temp, capacity, TOKEN_WORD))
		return ;
}

t_tokens	*ft_tokenizer(char *input, t_capacity *capacity)
{
	t_tokens	*tokens;

	capacity->max_size = 1;
	capacity->current_size = 0;
	tokens = malloc(sizeof(t_tokens) * capacity->max_size);
	if (tokens == NULL)
		return (NULL);
	while (*input)
	{
		while (ft_strchr(" \t\n", *input) && *input)
			input++;
		if (capacity->max_size <= capacity->current_size)
			tokens = ft_realloc_tokens_when_full(tokens, capacity);
		if (is_seperator(*input))
			handle_seperator(&input, tokens, capacity);
		else
			handle_word(&input, tokens, capacity);
	}
	print_tokens(tokens, capacity);
	return (tokens);
}
