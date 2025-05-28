# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: stefan <stefan@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/05/15 13:03:18 by anilchen          #+#    #+#              #
#    Updated: 2025/05/28 22:37:23 by stefan           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

CC     = c++
CFLAGS = -Wall -Wextra -Werror -I./includes -std=c++98

SRC_DIR = src
OBJ_DIR = obj

SRC += $(SRC_DIR)/main.cpp \

SRC += $(SRC_DIR)/User.cpp \
		$(SRC_DIR)/Server.cpp \
		$(SRC_DIR)/Channel.cpp \
		
SRC += $(SRC_DIR)/commands/AuthCommands.cpp \
		$(SRC_DIR)/commands/CommandHandler.cpp \
		$(SRC_DIR)/commands/ChannelCommands.cpp \
		$(SRC_DIR)/commands/MessagingCommands.cpp \

SRC += $(SRC_DIR)/command_utils/ReplyBuilder.cpp \
		$(SRC_DIR)/command_utils/CommandParser.cpp \

OBJ = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC))

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME) 
	
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
