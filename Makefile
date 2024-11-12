# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: upolat <upolat@student.hive.fi>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/10/12 16:42:23 by upolat            #+#    #+#              #
#    Updated: 2024/11/11 21:18:50 by upolat           ###   ########.fr        #
#    Updated: 2024/11/11 16:52:17 by hpirkola         ###   ########.fr        #
#    Updated: 2024/11/07 10:34:01 by upolat           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minishell

SRC_DIR = src
SRC_BONUS_DIR = src

SOURCES = 	$(SRC_DIR)/main.c $(SRC_DIR)/tokenizer.c $(SRC_DIR)/parser.c \
			$(SRC_DIR)/execution.c $(SRC_DIR)/path.c $(SRC_DIR)/globbing.c \
			$(SRC_DIR)/tokenization_utils.c $(SRC_DIR)/builtins.c \
		 	$(SRC_DIR)/ft_strdup2.c  $(SRC_DIR)/errors.c \

SOURCES_BONUS = $(SRC_DIR)/readline_test.c

OBJECTS = $(SOURCES:.c=.o)
OBJECTS_BONUS = $(SOURCES_BONUS:.c=.o)

CFLAGS = -Wall -Wextra -Werror # -fsanitize=address

libft_dir := library/libft
libft := $(libft_dir)/libft.a

READLINE_DIR := $(shell brew --prefix readline)

INCLUDES = -I./include -I$(libft_dir) -I$(READLINE_DIR)/include

all: $(NAME)

$(NAME): $(OBJECTS) $(libft)
	cc $(CFLAGS) $(INCLUDES) $(OBJECTS) \
		-L$(libft_dir) -lft \
		-L$(READLINE_DIR)/lib -lreadline \
		-Wl,-rpath,$(READLINE_DIR)/lib \
		-o $@

$(libft):
	$(MAKE) -C $(libft_dir)

bonus: .bonus

.bonus: $(OBJECTS_BONUS) $(libft)
	cc $(CFLAGS) $(INCLUDES) $(OBJECTS_BONUS) \
		-L$(libft_dir) -lft \
		-L$(READLINE_DIR)/lib -lreadline \
		-Wl,-rpath,$(READLINE_DIR)/lib \
		-o $(NAME)
	@touch .bonus

%.o: %.c
	cc $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(OBJECTS_BONUS) .bonus
	$(MAKE) -C $(libft_dir) clean

fclean: clean
	rm -f $(NAME) .here_doc
	$(MAKE) -C $(libft_dir) fclean

re: fclean all

rebonus: fclean bonus

.PHONY: all clean fclean re bonus rebonus
