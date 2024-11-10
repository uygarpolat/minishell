/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 11:16:11 by upolat            #+#    #+#             */
/*   Updated: 2024/11/10 03:11:06 by upolat           ###   ########.fr       */
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

void	init_signal(void)
{
	struct termios	term;

	// Is error handling necessary in any of this?
	tcgetattr(STDIN_FILENO, &term);
	term.c_lflag &= ~ECHOCTL;
	tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void	handle_sigint(void)
{
	write(STDOUT_FILENO, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

void	handle_sigquit(void)
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
	char	**temp_double_arr;
	if (node->type == AST_COMMAND)
	{
		temp_double_arr = node->words;
		//indent_level = -1;
		while (++indent_level < level + 1)
			printf("  ");
		printf("AST_COMMAND [TOKEN: ");
		while (*node->words)
		{
			//indent_level = -1;
			//while (++indent_level < level + 1)
			//	printf("  ");
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

int	is_seperator(char c, char c_plus_one)
{
	if (c == '<' || c == '>' || (c == '&' && c_plus_one == '&')
		|| c == '|' || c == '(' || c == ')')
		return (1);
	return (0);
}

void *free_tokens(t_tokens *tokens, t_capacity *capacity)
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

void	realloc_error(t_tokens *tokens, t_tokens *new_tokens, t_capacity *capacity, int i)
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
/*
char	*skip_a_char(char *str, char c)
{
	str++;
	while (*str && *str != c)
		str++;
	return (str);
}
*/
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
		//if (!*temp)
		//	break ;
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

int	encode_char_with_flag(char c)
{
	int	encoded_value;

	encoded_value = (1 << 8) | c;
	return (encoded_value);
}

char	*get_var(char *str, char **envp)
{
	if (!envp)
		return (NULL);
	while (*envp)
	{
		if (ft_strncmp(*envp, str, ft_strlen(str)) == 0
			&& *(*envp + ft_strlen(str)) == '=')
			return (*envp + ft_strlen(str) + 1);
		envp++;
	}
	return (NULL);
}

int	length_of_var(int **int_array, char **envp)
{
	int		i;
	int		n;
	int		len;
	char	*str;

	len = 0;
	i = 0;
	n = -1;
	//while (ft_isalnum((*int_array)[i]) || (*int_array)[i] == '_')
	//	i++;
	while ((ft_isalnum((*int_array)[i] & 0xFF) || ((*int_array)[i] & 0xFF) == '_') && !(((*int_array)[i] >> 8) & 1))
		i++;

	str = malloc(sizeof(char) * (i + 1));
	if (str == NULL)
		return (-1);
	while (++n < i)
		str[n] = (*int_array)[n];
	str[n] = '\0';
	if (get_var(str, envp) == NULL)
	{
		*int_array = *int_array + i;
		free_void((void **)&str, NULL);
		return (0);
	}
	len = ft_strlen(get_var(str, envp));
	*int_array = *int_array + i;
	if (str)
	{
		free(str);
		str = NULL;
	}
	return (len);
}

int	str_of_var(int **int_array_old, int **int_array_new, char **envp)
{
	int		i;
	int		n;
	char	*str;
	char	*var;

	i = 0;
	n = -1;
	//while (((*int_array & 0xFF) == '$') && ((*int_array >> 8) & 1) && *(int_array + 1))
	while ((ft_isalnum((*int_array_old)[i] & 0xFF) || ((*int_array_old)[i] & 0xFF) == '_') && !(((*int_array_old)[i] >> 8) & 1))
		i++;
	//while (ft_isalnum((*int_array_old)[i]) || (*int_array_old)[i] == '_')
	//	i++;
	str = malloc(sizeof(char) * (i + 1));
	if (str == NULL)
		return (-1);
	while (++n < i)
		str[n] = (*int_array_old)[n];
	str[n] = '\0';
	if (get_var(str, envp) == NULL)
	{
		*int_array_old = *int_array_old + i;
		free(str);
		str = NULL;
		return (1);
	}
	*int_array_old = *int_array_old + i;
	var = get_var(str, envp);
	free_void((void **)&str, NULL);
	while (*var)
	{
		**int_array_new = *var;
		(*int_array_new)++;
		var++;
	}
	return (0);
}

int	finalize_dollar_expansion(int *int_array_old,
		int **int_array_new, char **envp, int code)
{
	int	*temp;

	temp = *int_array_new;
	while (*int_array_old)
	{
		if (((*int_array_old & 0xFF) == '$') && ((*int_array_old >> 8) & 1) && *(int_array_old + 1))
		{
			if (*(int_array_old + 1) == '?')
			{
				char	*str_num;

				str_num = ft_itoa(code);
				int num = ft_strlen(str_num);
				while(*str_num)
				{
					**int_array_new = *str_num;
					(*int_array_new)++;
					str_num++;
				}
				int_array_old += 2;
				str_num = str_num - num;
				free_void((void **)&str_num, NULL);
			}
			else
			{
				int_array_old++;
				str_of_var(&int_array_old, int_array_new, envp);
			}
		}
		else
		{
			**int_array_new = *int_array_old;
			(*int_array_new)++;
			int_array_old++;
		}
	}
	*int_array_new = temp;
	return (0);
}

int	*expand_dollar(int *int_array, char **envp, int len, int num, int code)
{
	int		*arr;

	while (*int_array)
	{
		if (((*int_array & 0xFF) == '$') && ((*int_array >> 8) & 1) && *(int_array + 1))
		{
			if (*(int_array + 1) == '?')
			{
				char	*str_num;

				str_num = ft_itoa(code);
				num = ft_strlen(str_num);
				len = len + num;
				int_array += 2;
				free_void((void **)&str_num, NULL);
			}
			else
			{
				int_array++;
				num = length_of_var(&int_array, envp);
				if (num == -1)
					return (NULL); // Handle better.
				len = len + num;
			}
		}
		else
		{
			len++;
			int_array++;
		}
	}
	arr = malloc(sizeof(int) * (len + 1));
	if (arr == NULL)
		return (NULL);
	arr[len] = '\0';
	return (arr);
}
/*
int	finalize_dollar_expansion(int *int_array_old,
		int **int_array_new, char **envp, int code)
{
	int	*temp;

	temp = *int_array_new;
	while (*int_array_old)
	{
		if (((*int_array_old & 0xFF) == '$') && ((*int_array_old >> 8) & 1) && *(int_array_old + 1))
		{
			int_array_old++;
			str_of_var(&int_array_old, int_array_new, envp);
		}
		else
		{
			**int_array_new = *int_array_old;
			(*int_array_new)++;
			int_array_old++;
		}
	}
	*int_array_new = temp;
	return (0);
}

int	*expand_dollar(int *int_array, char **envp, int len, int num, int code)
{
	int		*arr;

	while (*int_array)
	{
		if (((*int_array & 0xFF) == '$') && ((*int_array >> 8) & 1) && *(int_array + 1))
		{
			int_array++;
			num = length_of_var(&int_array, envp);
			if (num == -1)
				return (NULL); // Handle better.
			len = len + num;
		}
		else
		{
			len++;
			int_array++;
		}
	}
	arr = malloc(sizeof(int) * (len + 1));
	if (arr == NULL)
		return (NULL);
	arr[len] = '\0';
	return (arr);
}
*/
void	assign_dollar(char *str, int *int_array, t_quote *q, int *m)
{
	if (q->single_q_count % 2 != 1 && (ft_isalnum(*(str + 1)) || (*(str + 1) == '_') || (*(str + 1) == '?')))
		int_array[*m] = encode_char_with_flag(*str);
	else
		int_array[*m] = *str;
	(*m)++;
}

void	assign_asterisk(char *str, int *int_array, t_quote *q, int *m)
{
	if (q->single_q_count % 2 == 0 && q->double_q_count % 2 == 0)
		int_array[*m] = encode_char_with_flag(*str);
	else
		int_array[*m] = *str;
	(*m)++;
}

void	assign_quote(char **str, int *int_array, t_quote *q, int *m, int flag)
{
	if (flag == 1)
		q->double_q_count++;
	else
		q->single_q_count++;
	if (ft_isalnum(*(*str + 1)) || *(*str + 1) == '_')
	{
		int_array[*m] = encode_char_with_flag(*(*str + 1));
		(*m)++;
	if (*(*str + 1) & 0xFF)
		(*str)++;
	}
}

int	populate_tokens(char *str, int *int_array)
{
	t_quote	q;
	int		m;

	m = 0;
	q.single_q_count = 0;
	q.double_q_count = 0;
	int_array[ft_strlen(str)] = 0;
	while (*str)
	{
		if (*str == '"' && q.single_q_count % 2 == 0)
		{
			assign_quote(&str, int_array, &q, &m, 1);
			//q.double_q_count++;
		}
		else if (*str == '\'' && q.double_q_count % 2 == 0)
		{
			assign_quote(&str, int_array, &q, &m, 0);
			//q.single_q_count++;
		}
		else if (*str == '$')
			assign_dollar(str, int_array, &q, &m);
		else if (*str == '*')
			assign_asterisk(str, int_array, &q, &m);
		else
			int_array[m++] = *str;
		str++;
	}
	int_array[m] = '\0';
	if (q.double_q_count % 2 != 0)
		return (ft_putstr_fd("unexpected EOF while looking for matching `\"'\n", 2), -1);
	if (q.single_q_count % 2 != 0)
		return (ft_putstr_fd("unexpected EOF while looking for matching `''\n", 2), -1);
	return (0);
}

int	handle_expansion_and_wildcard(t_tokens *tokens,
		t_capacity *capacity, char **envp, int code)
{
	int		i;
	int		*int_array;
	int		*int_array_new;

	int_array = NULL;
	int_array_new = NULL;
	i = -1;
	while (++i < capacity->current_size)
	{
		int_array = malloc(sizeof(int) * (ft_strlen(tokens[i].value) + 1));
		if (int_array == NULL)
			return (-1);
		int_array[ft_strlen(tokens[i].value)] = 0;
		if (populate_tokens(tokens[i].value, int_array))
			return (free_void((void **)&int_array, NULL), -1);
		int_array_new = expand_dollar(int_array, envp, 0, 0, code);
		finalize_dollar_expansion(int_array, &int_array_new, envp, code);
		free_void((void **)&tokens[i].value, NULL);
		tokens[i].value = expand_wildcard(int_array_new, tokens, i, 0);
		
		if (tokens[i].value == NULL)
		{
			free_void((void **)&int_array, NULL);
			free_void((void **)&int_array_new, NULL);
			return (-1);
		}
		free_void((void **)&int_array, NULL);
		free_void((void **)&int_array_new, NULL);
	}
	return (0);
}
/*

   "word", "(", ")", "<", ">", "|",
		"<<", ">>", "||", "&&"


first: word, open_paren, redir

before word: word, open_paren, redir, pipe, logic
after word: word, close_paren, redir, pipe, logic

before redir: word, open_paren, pipe, logic
after redir: word

before pipe: word, close_paren
after pipe: word, open_paren, redir

before logic: word, close_paren
after logic: word, open_paren, redir

before open paren: open_paren, logic, pipe
after open paren: word, open_paren, redir

before close_paren: word, close_paren
after close_paren: close_paren, pipe, logic

last: word, close_paren

*/

int	tokens_error_checker(t_tokens *tokens, t_capacity *capacity)
{
	int	i;

	if (tokens[0].type != TOKEN_WORD && !identify_token(tokens[0].type) && tokens[0].type != TOKEN_OPEN_PAREN)
	{
		syntax_error_near(tokens, 0);
		return (-1);
	}

	i = 0;
	while (i < capacity->current_size - 1)
	{
		if ((identify_token(tokens[i].type) && tokens[i + 1].type != TOKEN_WORD)
			|| (tokens[i].type == TOKEN_WORD && tokens[i + 1].type == TOKEN_OPEN_PAREN)
			|| (tokens[i].type == TOKEN_PIPE && !identify_token(tokens[i + 1].type) && tokens[i + 1].type != TOKEN_WORD && tokens[i + 1].type != TOKEN_OPEN_PAREN)
			|| (tokens[i].type == TOKEN_AND && !identify_token(tokens[i + 1].type) && tokens[i + 1].type != TOKEN_WORD && tokens[i + 1].type != TOKEN_OPEN_PAREN)
			|| (tokens[i].type == TOKEN_OR && !identify_token(tokens[i + 1].type) && tokens[i + 1].type != TOKEN_WORD && tokens[i + 1].type != TOKEN_OPEN_PAREN)
			|| (tokens[i].type == TOKEN_OPEN_PAREN && !identify_token(tokens[i + 1].type) && tokens[i + 1].type != TOKEN_WORD && tokens[i + 1].type != TOKEN_OPEN_PAREN)
			|| (tokens[i].type == TOKEN_CLOSE_PAREN && tokens[i + 1].type == TOKEN_AND && tokens[i + 1].type == TOKEN_OR && tokens[i + 1].type != TOKEN_PIPE && tokens[i + 1].type != TOKEN_CLOSE_PAREN)
			)
		{
			syntax_error_near(tokens, i + 1);
			return (-1);
		} 
		i++;
	}
	if (tokens[capacity->current_size - 1].type != TOKEN_WORD)
	{
		syntax_error_near(tokens, -1);
		return (-1);
	}
	return (0);
}

int	init_tokenizer(t_tokens **tokens, t_capacity *capacity)
{
	capacity->max_size = 1;
	capacity->current_size = 0;
	*tokens = malloc(sizeof(t_tokens) * capacity->max_size);
	if (*tokens == NULL)
		return (-1);
	(*tokens)->globbed = NULL;
	return (0);
}

t_tokens	*ft_tokenizer(char *input, t_capacity *capacity, char **envp, int code)
{
	t_tokens	*tokens;
	int			error_code;

	tokens = NULL;
	if (init_tokenizer(&tokens, capacity) == -1)
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
	//print_tokens(tokens, capacity);
	if (tokens_error_checker(tokens, capacity) == -1 || handle_expansion_and_wildcard(tokens, capacity, envp, code) == -1)
		return ((t_tokens *)free_tokens(tokens, capacity));
	return (tokens);
}
