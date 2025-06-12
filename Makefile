# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: stefan <stefan@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/05/15 13:03:18 by anilchen          #+#    #+#              #
#    Updated: 2025/06/12 13:34:56 by stefan           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv
NAME_BONUS = ircserv_bonus

CC     = c++
CFLAGS = -Wall -Wextra -Werror -I./includes -std=c++98
CFLAGS_BONUS = -Wall -Wextra -Werror -I./includes_bonus -std=c++98

SRC_DIR = src
SRC_BONUS_DIR   = src_bonus

OBJ_DIR = obj
OBJ_BONUS_DIR   = obj_bonus

BOT_SRC = bonus/weatherbot.cpp
BOT_BIN = bonus/weatherbot

# **************************************************************************** #
#                                mandatory                                     #
# **************************************************************************** #

SRC += $(SRC_DIR)/main.cpp \

SRC += $(SRC_DIR)/User.cpp \
		$(SRC_DIR)/Server.cpp \
		$(SRC_DIR)/Channel.cpp \
		$(SRC_DIR)/ServerUtils.cpp \
		$(SRC_DIR)/PollManager.cpp  \

		
SRC += $(SRC_DIR)/commands/AuthCommands.cpp \
		$(SRC_DIR)/commands/ServerCommands.cpp \
		$(SRC_DIR)/commands/CommandHandler.cpp \
		$(SRC_DIR)/commands/ChannelCommands.cpp \
		$(SRC_DIR)/commands/MessagingCommands.cpp \

SRC += $(SRC_DIR)/command_utils/ReplyBuilder.cpp \
		$(SRC_DIR)/command_utils/CommandParser.cpp \

# **************************************************************************** #
#                                	 bonus                                     #
# **************************************************************************** #	

SRCS_BONUS += $(SRC_BONUS_DIR)/main.cpp \

SRCS_BONUS += $(SRC_BONUS_DIR)/User.cpp \
			$(SRC_BONUS_DIR)/Server.cpp \
			$(SRC_BONUS_DIR)/Channel.cpp \
			$(SRC_BONUS_DIR)/ServerUtils.cpp \
			$(SRC_BONUS_DIR)/PollManager.cpp  \
			$(SRC_BONUS_DIR)/Bot.cpp  \

		
SRCS_BONUS += $(SRC_BONUS_DIR)/commands/AuthCommands.cpp \
			$(SRC_BONUS_DIR)/commands/ServerCommands.cpp \
			$(SRC_BONUS_DIR)/commands/CommandHandler.cpp \
			$(SRC_BONUS_DIR)/commands/ChannelCommands.cpp \
			$(SRC_BONUS_DIR)/commands/MessagingCommands.cpp \

SRCS_BONUS += $(SRC_BONUS_DIR)/command_utils/ReplyBuilder.cpp \
			$(SRC_BONUS_DIR)/command_utils/CommandParser.cpp \

OBJ = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC))
OBJ_BONUS = $(patsubst $(SRC_BONUS_DIR)/%.cpp, $(OBJ_BONUS_DIR)/%.o, $(SRCS_BONUS))

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME) 

bonus: $(NAME_BONUS)

$(NAME_BONUS): $(OBJ_BONUS)
	$(CC) $(CFLAGS_BONUS) $(OBJ_BONUS) -o $(NAME_BONUS) 
	
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_BONUS_DIR)/%.o: $(SRC_BONUS_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS_BONUS) -c $< -o $@

#$(OBJ_DIR):
#	mkdir -p $(OBJ_DIR)

bot: $(BOT_BIN)

$(BOT_BIN): $(BOT_SRC)
	$(CC) $(CFLAGS) -o $(BOT_BIN) $(BOT_SRC)

clean:
	rm -rf $(OBJ_DIR) $(OBJ_BONUS_DIR)

fclean: clean
	rm -f $(NAME) $(NAME_BONUS)
	rm -f $(BOT_BIN)

re: fclean all

rebonus: fclean bonus

.PHONY: all bonus clean fclean re rebonus
