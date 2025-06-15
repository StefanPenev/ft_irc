# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: stefan <stefan@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/06/14 21:49:31 by stefan            #+#    #+#              #
#    Updated: 2025/06/14 21:53:27 by stefan           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# ============================================================================ #
#                                CONFIGURATION								   #
# ============================================================================ #
NAME            = ircserv
NAME_BONUS      = ircserv_bonus
BOT_BIN         = weatherbot

CC              = c++
CFLAGS          = -Wall -Wextra -Werror -std=c++98
INCLUDES        = -I./includes
CPPFLAGS        = -MMD -MP $(INCLUDES)

SRC_DIR         = src
OBJ_DIR         = obj

BOT_SRC_DIR     = bonus
BOT_OBJ_DIR     = obj_bonus

# ============================================================================ #
#                              SOURCE FILES									   #
# ============================================================================ #

SRC = \
	$(SRC_DIR)/main.cpp \
	$(SRC_DIR)/User.cpp \
	$(SRC_DIR)/Server.cpp \
	$(SRC_DIR)/Channel.cpp \
	$(SRC_DIR)/ServerUtils.cpp \
	$(SRC_DIR)/PollManager.cpp \
	$(SRC_DIR)/commands/AuthCommands.cpp \
	$(SRC_DIR)/commands/ServerCommands.cpp \
	$(SRC_DIR)/commands/CommandHandler.cpp \
	$(SRC_DIR)/commands/ChannelCommands.cpp \
	$(SRC_DIR)/commands/MessagingCommands.cpp \
	$(SRC_DIR)/command_utils/ReplyBuilder.cpp \
	$(SRC_DIR)/command_utils/CommandParser.cpp

OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
DEP = $(OBJ:.o=.d)

BOT_SRC = \
	bonus/main.cpp \
	bonus/WeatherBot.cpp

BOT_OBJ = $(BOT_SRC:bonus/%.cpp=$(BOT_OBJ_DIR)/%.o)
BOT_DEP = $(BOT_OBJ:.o=.d)

# ============================================================================ #
#                                BUILD RULES								   #
# ============================================================================ #
all: $(BIN_DIR) $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -c $< -o $@

# Bonus target
bot: $(BOT_BIN)

$(BOT_BIN): $(BOT_OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(BOT_OBJ) -o $@

$(BOT_OBJ_DIR)/%.o: bonus/%.cpp
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -c $< -o $@

# Debug build
debug: CFLAGS += -g -DDEBUG
debug: re

# ============================================================================ #
#                               CLEANING RULES								   #
# ============================================================================ #
clean:
	rm -rf $(OBJ_DIR) $(BOT_OBJ_DIR)

fclean: clean
	rm -f $(NAME) $(NAME_BONUS) $(BOT_BIN)

re: fclean all

# ============================================================================ #
#                        INCLUDE AUTO-GENERATED DEPS						   #
# ============================================================================ #
-include $(DEP)
-include $(BOT_DEP)

# ============================================================================ #
#                                PHONY TARGETS								   #
# ============================================================================ #
.PHONY: all clean fclean re debug bot