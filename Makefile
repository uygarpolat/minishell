# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/10/12 16:42:23 by upolat            #+#    #+#              #
#    Updated: 2025/01/24 13:19:45 by upolat           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minishell

SRC_DIR = 				src
TOKENIZER_DIR =			$(SRC_DIR)/01-tokenizer
PARSER_DIR =			$(SRC_DIR)/02-parser
SIGNALS_DIR =			$(SRC_DIR)/03-signals
VISUALIZATION_DIR =		$(SRC_DIR)/04-visualization
EXECUTION_DIR =			$(SRC_DIR)/05-execution

SOURCES = 	$(SRC_DIR)/main.c \
			\
			$(TOKENIZER_DIR)/tokenizer.c $(TOKENIZER_DIR)/tokenization_utils.c \
			$(TOKENIZER_DIR)/error_and_grammar.c $(TOKENIZER_DIR)/malloc_and_free.c \
			$(TOKENIZER_DIR)/expansion_and_wildcard.c $(TOKENIZER_DIR)/handle_word_and_separator.c \
			$(TOKENIZER_DIR)/expand_dollar.c $(TOKENIZER_DIR)/expand_dollar_2.c \
			$(TOKENIZER_DIR)/populate_tokens.c $(TOKENIZER_DIR)/globbing.c \
			\
			$(PARSER_DIR)/parser.c $(PARSER_DIR)/parser_utils.c \
			$(PARSER_DIR)/populate_command_node.c $(PARSER_DIR)/memory_and_error.c \
			\
			$(SIGNALS_DIR)/signals.c $(SIGNALS_DIR)/signals_utils.c \
			\
			$(VISUALIZATION_DIR)/print_ast.c $(VISUALIZATION_DIR)/print_tokens.c \
			$(VISUALIZATION_DIR)/print_welcome.c \
			\
			$(EXECUTION_DIR)/execution.c $(EXECUTION_DIR)/path.c \
			$(EXECUTION_DIR)/execution_utils.c $(EXECUTION_DIR)/execution_utils_2.c \
			$(EXECUTION_DIR)/builtins.c $(EXECUTION_DIR)/env_export.c \
		 	$(EXECUTION_DIR)/ft_strdup2.c $(EXECUTION_DIR)/errors.c \
			$(EXECUTION_DIR)/ft_atol.c  $(EXECUTION_DIR)/files.c $(EXECUTION_DIR)/functions.c \
			$(EXECUTION_DIR)/heredoc.c $(EXECUTION_DIR)/exec_builtin.c $(EXECUTION_DIR)/pipes.c \
			$(EXECUTION_DIR)/var.c $(EXECUTION_DIR)/env.c $(EXECUTION_DIR)/error_checks.c \

OBJECTS = $(SOURCES:.c=.o)

CFLAGS = -Wall -Wextra -Werror -g

libft_dir := library/libft
libft := $(libft_dir)/libft.a

READLINE_DIR := $(shell brew --prefix readline)

INCLUDES = -I./include -I$(libft_dir) -I$(READLINE_DIR)/include

san: CFLAGS += -fsanitize=address,undefined

all: $(NAME)

$(NAME): $(OBJECTS) $(libft)
	cc $(CFLAGS) $(INCLUDES) $(OBJECTS) -L$(libft_dir) -lft -L$(READLINE_DIR)/lib -lreadline -Wl,-rpath,$(READLINE_DIR)/lib -o $@

$(libft):
	$(MAKE) -C $(libft_dir)

bonus: $(NAME)

%.o: %.c
	cc $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJECTS)
	$(MAKE) -C $(libft_dir) clean

fclean: clean
	rm -f $(NAME) .here_doc
	$(MAKE) -C $(libft_dir) fclean

leaks: fclean $(NAME)
	leaks -q --atExit ./$(NAME) --

re: fclean all

san: re

rebonus: fclean bonus

.PHONY: all clean fclean re bonus rebonus
