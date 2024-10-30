/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 11:16:11 by upolat            #+#    #+#             */
/*   Updated: 2024/10/30 15:18:05 by upolat           ###   ########.fr       */
/*   Updated: 2024/10/30 13:46:02 by hpirkola         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../library/libft/libft.h"
#include "../includes/tokenizer.h"
#include "../includes/ast.h"
#include "termios.h"

char	*expand_wildcard(int *int_array);

void	init_signal()
{
	struct termios	term;

	// Is error handling necessary in any of this?
	tcgetattr(STDIN_FILENO, &term); // Gets current terminal attributes
    term.c_lflag &= ~ECHOCTL;       // Unsets the ECHOCTL flag so that signals aren't printed, like ^C
    tcsetattr(STDIN_FILENO, TCSANOW, &term); // Applies changes immediately
}

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
	if (!tokens)
		return ;
	while (i < capacity->max_size)
	{
		if (tokens[i].value)
			free(tokens[i].value);
		tokens[i].value = NULL;
		i++;
	}
	if (tokens)
	{
		free(tokens);
		tokens = NULL;
	}
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
	while (++i < capacity->max_size * 2)
	{
		if (i < capacity->current_size)
		{
			new_tokens[i].value = ft_strdup(tokens[i].value);
			new_tokens[i].type = tokens[i].type;
		}
		else
			new_tokens[i].value = NULL;
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
	//if (*str)
	//	str++;
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
			*temp = skip_a_char(*temp, '"');
		else if (**temp == '\'')
			*temp = skip_a_char(*temp, '\'');
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
	while (*temp && skip_quotes_and_ampersand(&temp))
		temp++;

	if (malloc_individual_tokens(tokens, input, temp, capacity, TOKEN_WORD))
		return ;
}

char	*append_char(char *str, char c)
{
	char	*new_str;
	int		len;
	int		i;

	len = 0;
	if (str != NULL)
	{
		while (str[len] != '\0')
			len++;
	}
	new_str = (char *)malloc((len + 2) * sizeof(char));
	if (new_str == NULL)
	{
		free(str);
		return (NULL);
	}
	i = 0;
	while (i < len)
	{
		new_str[i] = str[i];
		i++;
	}
	new_str[i++] = c;
	new_str[i] = '\0';
	free(str);
	str = NULL;
	return (new_str);
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
	while (ft_isalnum((*int_array)[i]) || (*int_array)[i] == '_')
		i++;
	str = malloc(sizeof(char) * (i + 1));
	if (str == NULL)
		return (-1);
	while (++n < i)
		str[n] = (*int_array)[n];
	str[n] = '\0';
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
	while (ft_isalnum((*int_array_old)[i]) || (*int_array_old)[i] == '_')
		i++;
	str = malloc(sizeof(char) * (i + 1));
	if (str == NULL)
		return (-1);
	while (++n < i)
		str[n] = (*int_array_old)[n];
	str[n] = '\0';
	*int_array_old = *int_array_old + i;
	var = get_var(str, envp);
	free(str);
	while (*var)
	{
		**int_array_new = *var;
		(*int_array_new)++;
		var++;
	}
	return (0);
}

int	finalize_dollar_expansion(int *int_array_old, int **int_array_new, char **envp)
{
	int	*temp;

	temp = *int_array_new;
	while (*int_array_old)
	{
		if (((*int_array_old & 0xFF) == '$') && ((*int_array_old >> 8) & 1))
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

int	*expand_dollar(int *int_array, char **envp)
{
	int		*arr;
	int		len;
	int		num;

	len = 0;
	num = 0;
	while (*int_array)
	{
		if (((*int_array & 0xFF) == '$') && ((*int_array >> 8) & 1))
		{
			int_array++;
			num = length_of_var(&int_array, envp);
			if (num == -1)
				return (0); // This is for malloc check in length_of_var function, handle better.
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

int	populate_tokens(char *str, int *int_array)
{
	int		single_q_count;
	int		double_q_count;
	int		m;

	m = 0;
	single_q_count = 0;
	double_q_count = 0;
	int_array[ft_strlen(str)] = 0;
	while (*str)
	{
		if (*str == '"' && single_q_count % 2 == 0)
			double_q_count++;
		else if (*str == '\'' && double_q_count % 2 == 0)
			single_q_count++;
		else if (*str == '$')
		{
			if (single_q_count % 2 != 1)
				int_array[m++] = encode_char_with_flag(*str);
			else
				int_array[m++] = *str;
		}
		else if (*str == '*')
		{
			if (single_q_count % 2 == 0 && double_q_count % 2 == 0)
				int_array[m++] = encode_char_with_flag(*str);
			else
				int_array[m++] = *str;
		}
		else
			int_array[m++] = *str;
		str++;
	}
	int_array[m] = '\0';
	if (single_q_count % 2 != 0 || double_q_count % 2 != 0)
		return (-1);
	return (0);
}

int	which_quote_mode(t_tokens *tokens, t_capacity *capacity, char **envp)
{
	int		i;
	//int		j;
	//int		m;
	//int		single_q_count;
	//int		double_q_count;
	//int		flag;
	int		*int_array;
	int		*int_array_new;

	//single_q_count = 0;
	//double_q_count = 0;
	//flag = 0;
	int_array = NULL;
	int_array_new = NULL;
	i = 0;
	while (i < capacity->current_size)
	{
		//single_q_count = 0;
		//double_q_count = 0;
		//flag = 0;
		//j = 0;
		//m = 0;
		int_array = malloc(sizeof(int) * (ft_strlen(tokens[i].value) + 1));
		if (int_array == NULL)
			return (0);
		int_array[ft_strlen(tokens[i].value)] = 0;
		if (populate_tokens(tokens[i].value, int_array))
			return (printf("Unmatched quotes error!\n"), -1);

/*		while (tokens[i].value[j])
		{
			if (tokens[i].value[j] == '"' && single_q_count % 2 == 0)
				double_q_count++;
			else if (tokens[i].value[j] == '\'' && double_q_count % 2 == 0)
				single_q_count++;
			else if (tokens[i].value[j] == '$')
			{
				if (single_q_count % 2 != 1)
					int_array[m++] = encode_char_with_flag(tokens[i].value[j]);
				else
					int_array[m++] = tokens[i].value[j];
			}
			else if (tokens[i].value[j] == '*')
			{
				if (single_q_count % 2 == 0 && double_q_count % 2 == 0)
					int_array[m++] = encode_char_with_flag(tokens[i].value[j]);
				else
					int_array[m++] = tokens[i].value[j];
			}
			else
				int_array[m++] = tokens[i].value[j];
			j++;
		}
		int_array[m] = '\0'; */
		int_array_new = expand_dollar(int_array, envp);
		finalize_dollar_expansion(int_array, &int_array_new, envp);
		if (tokens[i].value)
		{
			free(tokens[i].value);
			tokens[i].value = NULL;
		}
		tokens[i].value = expand_wildcard(int_array_new);
		free_void((void **)&int_array, NULL);
		free_void((void **)&int_array_new, NULL);
		//if (single_q_count % 2 != 0 || double_q_count % 2 != 0)
		//	flag = -1;
		//if (flag == -1)
		//	return (printf("Unmatched quotes error!\n"), -1);
		i++;
	}
	return (0);
}

t_tokens	*ft_tokenizer(char *input, t_capacity *capacity, char **envp)
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
	if (which_quote_mode(tokens, capacity, envp) == -1)
		return (NULL); // Make sure to free the tokens.
	return (tokens);
}
