NAME        = ft_ping
CC          = gcc
CFLAGS      = -Wall -Wextra -Werror -Iinclude
SRC_DIR     = src
OBJ_DIR     = obj
SRC         = main.c parse.c socket.c icmp.c ping.c stats.c signal.c utils.c
OBJ         = $(addprefix $(OBJ_DIR)/, $(SRC:.c=.o))

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ) -lm

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
