/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 11:16:11 by upolat            #+#    #+#             */
/*   Updated: 2024/11/15 15:55:23 by upolat           ###   ########.fr       */
/*   Updated: 2024/11/07 12:36:19 by hpirkola         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../library/libft/libft.h"
#include "../includes/tokenizer.h"
#include "../includes/ast.h"
#include "termios.h"

int		populate_command_node_error_check(t_tokens *tokens, int start, int *end);
int		identify_token(t_token_type type);
void	syntax_error_near(t_tokens *tokens, int loc);
char	*expand_wildcard(int *int_array, t_tokens *tokens, int loc, int flag);

extern int	debug_flag; // DELETE BEFORE PRODUCTION!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

void	print_tokens(t_tokens *tokens, t_capacity *capacity, int debug_flag)
{
	if (debug_flag < 2)
		return ;
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
	printf("----------\n");
}

void	print_ast(t_ast *node, int level, int debug_flag)
{
	if (debug_flag < 2)
		return ;
	int		indent_level;
	char	**temp_double_arr;

	if (node == NULL)
		return ;
	indent_level = -1;
	while (++indent_level < level)
		printf("  ");
	if (node->type == AST_COMMAND)
	{
		temp_double_arr = node->words;
		while (++indent_level < level + 1)
			printf("  ");
		printf("AST_COMMAND [TOKEN: ");
		while (*node->words)
		{
			printf("%s ", *(node->words));
			(node->words)++;
		}
		printf("]\n");
		node->words = temp_double_arr;
		while (*node->words)
		{
			indent_level = -1;
			while (++indent_level < level + 1)
				printf("  ");
			printf("[TOKEN: %s]\n", *(node->words));
			(node->words)++;
		}

		node->words = temp_double_arr;
	}
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
		print_ast(node->left, level + 1, debug_flag);
	if (node->right != NULL)
		print_ast(node->right, level + 1, debug_flag);
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

void	handle_sigint(int signum)
{
	(void)signum;
	g_signal = 1;
	ft_putstr_fd("\n", STDOUT_FILENO);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

void	handle_sigquit(int signum)
{
	(void)signum;
}

int	init_signal(int argc, char **argv)
{
	struct termios	term;

	(void)argc;
	(void)argv;
	tcgetattr(STDIN_FILENO, &term);
	term.c_lflag &= ~ECHOCTL;
	tcsetattr(STDIN_FILENO, TCSANOW, &term);
	signal(SIGINT, handle_sigint);
	signal(SIGQUIT, handle_sigquit);
	return (0);
}

int	is_seperator(char c, char c_plus_one)
{
	if (c == '<' || c == '>' || (c == '&' && c_plus_one == '&')
		|| c == '|' || c == '(' || c == ')')
		return (1);
	return (0);
}

void	*free_tokens(t_tokens *tokens, t_capacity *capacity)
{
	int	i;

	i = 0;
	if (!tokens)
		return (tokens);
	while (i < capacity->max_size)
	{
		free_void((void **)&tokens[i].value, NULL);
		free_2d_array((void ***)&tokens[i].globbed);
		i++;
	}
	free_void((void **)&tokens, NULL);
	return ((void *)tokens);
}

void	realloc_error(t_tokens *tokens, t_tokens *new_tokens,
			t_capacity *capacity, int i)
{
	free_tokens(tokens, capacity);
	capacity->max_size *= 2;
	while (++i < capacity->max_size)
		new_tokens[i].value = NULL;
	free_tokens(new_tokens, capacity);
}

t_tokens	*realloc_tokens_when_full(t_tokens *tokens,
		t_capacity *capacity, int i)
{
	t_tokens	*new_tokens;

	new_tokens = malloc(sizeof(t_tokens) * (capacity->max_size * 2));
	if (new_tokens == NULL)
		return ((t_tokens *)free_tokens(tokens, capacity));
	while (++i < capacity->max_size * 2)
	{
		if (i < capacity->current_size)
		{
			new_tokens[i].value = ft_strdup(tokens[i].value);
			if (new_tokens[i].value == NULL)
				return (realloc_error(tokens, new_tokens, capacity, i), NULL);
			new_tokens[i].globbed = NULL;
			new_tokens[i].type = tokens[i].type;
			new_tokens[i].code = tokens[i].code;
		}
		else
		{
			new_tokens[i].value = NULL;
			new_tokens[i].globbed = NULL;
		}
	}
	free_tokens(tokens, capacity);
	capacity->max_size *= 2;
	return (new_tokens);
}

void	assign_token_types(char **temp, t_token_type *type,
			t_token_type type1, t_token_type type2)
{
	(*temp)++;
	if (type2 != TOKEN_UNKNOWN && **temp == *(*temp - 1))
	{
		(*temp)++;
		*type = type2;
	}
	else
		*type = type1;
}

int	malloc_individual_tokens(t_tokens *tokens, char **input,
		char *temp, t_capacity *capacity)
{
	tokens[capacity->current_size].value
		= malloc(sizeof(char) * (temp - *input + 1));
	if (tokens[capacity->current_size].value == NULL)
		return (free_void((void **)&tokens, NULL), -1);
	ft_strlcpy(tokens[capacity->current_size].value, *input, temp - *input + 1);
	capacity->current_size++;
	*input = temp;
	return (0);
}

int	handle_seperator(char **input, t_tokens *tokens, t_capacity *capacity)
{
	char			*temp;
	t_token_type	type;

	temp = *input;
	if (*temp == '<')
		assign_token_types(&temp, &type, TOKEN_REDIR_IN, TOKEN_HEREDOC);
	else if (*temp == '>')
		assign_token_types(&temp, &type, TOKEN_REDIR_OUT, TOKEN_APPEND);
	else if (*temp == '|')
		assign_token_types(&temp, &type, TOKEN_PIPE, TOKEN_OR);
	else if (*temp == '&' && *(temp + 1) == '&')
	{
		temp += 2;
		type = TOKEN_AND;
	}
	else if (*temp == '(')
		assign_token_types(&temp, &type, TOKEN_OPEN_PAREN, TOKEN_UNKNOWN);
	else
		assign_token_types(&temp, &type, TOKEN_CLOSE_PAREN, TOKEN_UNKNOWN);
	tokens[capacity->current_size].type = type;
	if (malloc_individual_tokens(tokens, input, temp, capacity))
		return (-1);
	return (0);
}

char	*skip_a_char(char *str, char c)
{
	char	*temp;
	int		flag;

	temp = NULL;
	flag = 0;
	str++;
	while (*str && *str != c)
	{
		if (ft_strchr(" \n\t<>|&()\"'", *str) && !flag)
		{
			flag = 1;
			temp = str - 1;
		}
		str++;
	}
	if (*str)
		return (str);
	else if (temp != NULL)
		return (temp);
	return (str);
}

int	skip_quotes(char **temp)
{
	if (ft_strchr(" \n\t<>|&()\"'", **temp))
	{
		if (**temp == '"')
			*temp = skip_a_char(*temp, '"');
		else if (**temp == '\'')
			*temp = skip_a_char(*temp, '\'');
		else
			return (0);
	}
	return (1);
}

int	handle_word(char **input, t_tokens *tokens, t_capacity *capacity)
{
	char	*temp;

	temp = *input;
	if (!*temp)
		return (0);
	while (*temp)
	{
		while (*temp && skip_quotes(&temp))
		{
			if (*temp)
				temp++;
		}
		if (*temp == '&')
		{
			if (*(temp + 1) == '&')
				break ;
			temp++;
		}
		else
			break ;
	}
	tokens[capacity->current_size].type = TOKEN_WORD;
	if (malloc_individual_tokens(tokens, input, temp, capacity))
		return (-1);
	return (0);
}

int	encode_char_with_flag(int c, int shift_amount)
{
	int	encoded_value;

	encoded_value = (1 << shift_amount) | c;
	return (encoded_value);
}

void	assign_dollar(char *str, int *int_array, t_quote *q, int *m)
{
	if (q->single_q_count % 2 != 1)
	{
		if (ft_isalnum(*(str + 1)) || (*(str + 1) == '_')
			|| (*(str + 1) == '?'))
		{
			int_array[*m] = encode_char_with_flag(*str, 9);
			if (q->double_q_count % 2 == 1)
				int_array[*m] = encode_char_with_flag(int_array[*m], 8);
		}
		else if ((*(str + 1) == '"' || *(str + 1) == '\'')
			&& q->double_q_count % 2 != 1)
			return ;
		else
			int_array[*m] = *str;
	}
	else
		int_array[*m] = *str;
	(*m)++;
}

void	assign_asterisk(char *str, int *int_array, t_quote *q, int *m)
{
	if (q->single_q_count % 2 == 0 && q->double_q_count % 2 == 0)
		int_array[*m] = encode_char_with_flag(*str, 9);
	else
		int_array[*m] = *str;
	(*m)++;
}

void	assign_quote(char **str, int *int_array, t_quote *q, int *m)
{
	if (**str == '"')
	{
		if (q->double_q_count % 2 == 0)
			q->double_first_unclosed = *str;
		else
			q->double_first_unclosed = q->str_initial;
		q->double_q_count++;
	}
	else
	{
		if (q->single_q_count % 2 == 0)
			q->single_first_unclosed = *str;
		else
			q->single_first_unclosed = q->str_initial;
		q->single_q_count++;
	}
	if (ft_isalnum(*(*str + 1) & 0xFF) || (*(*str + 1) & 0xFF) == '_')
	{
		int_array[*m] = encode_char_with_flag(*(*str + 1), 9);
		(*m)++;
		if (*(*str + 1) & 0xFF)
			(*str)++;
	}
}

void	init_populate_tokens(t_quote *q, char *str)
{
	q->single_q_count = 0;
	q->double_q_count = 0;
	q->single_first_unclosed = &str[ft_strlen(str)];
	q->double_first_unclosed = &str[ft_strlen(str)];
	q->str_initial = &str[ft_strlen(str)];
}

int	populate_tokens(char *str, int *code, int *int_array, int m)
{
	t_quote	q;

	init_populate_tokens(&q, str);
	while (*str)
	{
		if (*str == '"' && q.single_q_count % 2 == 0)
			assign_quote(&str, int_array, &q, &m);
		else if (*str == '\'' && q.double_q_count % 2 == 0)
			assign_quote(&str, int_array, &q, &m);
		else if (*str == '$')
			assign_dollar(str, int_array, &q, &m);
		else if (*str == '*')
			assign_asterisk(str, int_array, &q, &m);
		else
			int_array[m++] = *str;
		str++;
	}
	if (q.double_first_unclosed < q.single_first_unclosed)
		return (error_handler(NULL, "unexpected EOF "
				"while looking for matching `\"'", code, 2), -1);
	else if (q.double_first_unclosed > q.single_first_unclosed)
		return (error_handler(NULL, "unexpected EOF "
				"while looking for matching `''", code, 2), -1);
	return (0);
}

void	init_arrays(t_tokens *tokens, t_arrays *a, char **envp)
{
	a->int_array_new = NULL;
	a->int_array_old = NULL;
	a->int_array_new_start = NULL;
	a->int_array_old_start = NULL;
	a->envp = envp;
	a->code = tokens->code;
}

void	free_int_arrays(t_arrays *a)
{
	free_void((void **)&a->int_array_old_start, NULL);
	free_void((void **)&a->int_array_new_start, NULL);
}

int	handle_expansion_and_wildcard(t_tokens *tokens,
		t_capacity *capacity, char **envp)
{
	int				i;
	t_token_type	type;
	t_arrays		a;

	init_arrays(tokens, &a, envp);
	i = -1;
	while (++i < capacity->current_size)
	{
		type = TOKEN_UNKNOWN;
		a.int_array_old = ft_calloc((ft_strlen(tokens[i].value)
					+ 1), sizeof(int));
		if (a.int_array_old == NULL)
			return (-1);
		if (populate_tokens(tokens[i].value, tokens->code, a.int_array_old, 0))
			return (free_void((void **)&a.int_array_old, NULL), -1);
		a.int_array_old_start = a.int_array_old;
		if (i > 0 && identify_token(tokens[i - 1].type))
			type = tokens[i - 1].type;
		a.int_array_new = ultimate_dollar_expansion(&a, type, 0, 0);
		if (a.int_array_new == NULL)
			return (free_int_arrays(&a), -1);
		a.int_array_new_start = a.int_array_new;
		a.int_array_old = a.int_array_old_start;
		if (ultimate_dollar_expansion(&a, type, 1, 0) == NULL)
			return (free_int_arrays(&a), -1);
		a.int_array_new = a.int_array_new_start;
		free_void((void **)&tokens[i].value, NULL);
		tokens[i].value = expand_wildcard(a.int_array_new, tokens, i, 0);
		free_int_arrays(&a);
		if (tokens[i].value == NULL)
			return (-1);
	}
	return (0);
}

int	tokens_grammar(t_tokens *tokens, t_capacity *capacity, int i)
{
	while (++i < capacity->current_size - 1)
	{
		if ((identify_token(tokens[i].type) && tokens[i + 1].type
				!= TOKEN_WORD)
			|| (tokens[i].type == TOKEN_WORD && tokens[i + 1].type
				== TOKEN_OPEN_PAREN) || (tokens[i].type == TOKEN_PIPE
				&& !identify_token(tokens[i + 1].type) && tokens[i + 1].type
				!= TOKEN_WORD && tokens[i + 1].type != TOKEN_OPEN_PAREN)
			|| (tokens[i].type
				== TOKEN_AND && !identify_token(tokens[i + 1].type)
				&& tokens[i + 1].type != TOKEN_WORD && tokens[i + 1].type
				!= TOKEN_OPEN_PAREN) || (tokens[i].type == TOKEN_OR
				&& !identify_token(tokens[i + 1].type) && tokens[i + 1].type
				!= TOKEN_WORD && tokens[i + 1].type
				!= TOKEN_OPEN_PAREN) || (tokens[i].type == TOKEN_OPEN_PAREN
				&& !identify_token(tokens[i + 1].type) && tokens[i + 1].type
				!= TOKEN_WORD && tokens[i + 1].type != TOKEN_OPEN_PAREN)
			|| (tokens[i].type == TOKEN_CLOSE_PAREN
				&& tokens[i + 1].type != TOKEN_AND && tokens[i + 1].type
				!= TOKEN_OR && tokens[i + 1].type != TOKEN_PIPE
				&& tokens[i + 1].type != TOKEN_CLOSE_PAREN))
			return (syntax_error_near(tokens, i + 1), -1);
	}
	return (0);
}

int	tokens_error_checker(t_tokens *tokens, t_capacity *capacity)
{
	int	i;
	int	k;

	if (tokens[0].type != TOKEN_WORD && !identify_token(tokens[0].type)
		&& tokens[0].type != TOKEN_OPEN_PAREN)
		return (syntax_error_near(tokens, 0), -1);
	if (tokens_grammar(tokens, capacity, -1) == -1)
		return (-1);
	if (tokens[capacity->current_size - 1].type != TOKEN_WORD
		&& tokens[capacity->current_size - 1].type != TOKEN_CLOSE_PAREN)
		return (syntax_error_near(tokens, -1), -1);
	i = 0;
	k = 0;
	while (i < capacity->current_size)
	{
		if (tokens[i].type == TOKEN_OPEN_PAREN
			|| tokens[i].type == TOKEN_CLOSE_PAREN)
			k = find_matching_paren(tokens, i, capacity->current_size - 1);
		if (k < 0)
			return (syntax_error_near(tokens, i), -1);
		if (k > i)
			i = k;
		i++;
	}
	return (0);
}

int	init_tokenizer(t_tokens **tokens, t_capacity *capacity, int *code)
{
	capacity->max_size = 1;
	capacity->current_size = 0;
	*tokens = malloc(sizeof(t_tokens) * capacity->max_size);
	if (*tokens == NULL)
		return (-1); // User error handler here
	(*tokens)->globbed = NULL;
	(*tokens)->code = code;
	return (0);
}

t_tokens	*ft_tokenizer(char *input, t_capacity *capacity,
				char **envp, int *code)
{
	t_tokens	*tokens;
	int			error_code;

	tokens = NULL;
	if (init_tokenizer(&tokens, capacity, code) == -1)
		return (NULL);
	while (*input)
	{
		while (ft_strchr(" \t\n", *input) && *input)
			input++;
		if (capacity->max_size <= capacity->current_size)
			tokens = realloc_tokens_when_full(tokens, capacity, -1);
		if (tokens == NULL)
			return (NULL);
		if (*input && is_seperator(*input, *(input + 1)))
			error_code = handle_seperator(&input, tokens, capacity);
		else if (*input)
			error_code = handle_word(&input, tokens, capacity);
		if (error_code == -1)
			return (NULL);
	}
	print_tokens(tokens, capacity, debug_flag);
	if (tokens_error_checker(tokens, capacity) == -1
		|| handle_expansion_and_wildcard(tokens, capacity, envp) == -1)
		return ((t_tokens *)free_tokens(tokens, capacity));
	return (tokens);
}
