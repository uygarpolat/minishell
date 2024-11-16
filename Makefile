# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/10/12 16:42:23 by upolat            #+#    #+#              #
#    Updated: 2024/11/17 01:55:06 by upolat           ###   ########.fr        #
#    Updated: 2024/11/11 16:52:17 by hpirkola         ###   ########.fr        #
#    Updated: 2024/11/07 10:34:01 by upolat           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minishell

SRC_DIR = 				src
TOKENIZER_DIR =			$(SRC_DIR)/01-tokenizer
PARSER_DIR =			$(SRC_DIR)/02-parser
SIGNALS_DIR =			$(SRC_DIR)/03-signals
VISUALIZATION_DIR =		$(SRC_DIR)/04-visualization

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
			$(SIGNALS_DIR)/signals.c \
			\
			$(VISUALIZATION_DIR)/print_ast.c $(VISUALIZATION_DIR)/print_tokens.c \
			\
			$(SRC_DIR)/execution.c $(SRC_DIR)/path.c \
			$(SRC_DIR)/builtins.c \
		 	$(SRC_DIR)/ft_strdup2.c $(SRC_DIR)/errors.c \

OBJECTS = $(SOURCES:.c=.o)

CFLAGS = -Wall -Wextra -Werror # -g

libft_dir := library/libft
libft := $(libft_dir)/libft.a

READLINE_DIR := $(shell brew --prefix readline)

INCLUDES = -I./include -I$(libft_dir) -I$(READLINE_DIR)/include

target san: CFLAGS += -fsanitize=address,undefined

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

re: fclean all

san: re

rebonus: fclean bonus

.PHONY: all clean fclean re bonus rebonus
