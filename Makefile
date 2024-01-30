#******************************************************************************#
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/08/09 15:08:49 by vavaas            #+#    #+#              #
#    Updated: 2024/01/30 02:40:31 by vvaas            ###   ########.fr        #
#                                                                              #
#******************************************************************************#

NAME	= ircserv

NAME_BONUS = greg

SRCS	= srcs/main.cpp     \
		  srcs/logs.cpp      \
		  srcs/ansi.cpp       \
		  srcs/server.cpp      \
		  srcs/client.cpp       \
		  srcs/channel.cpp       \
		  srcs/message.cpp        \
		  srcs/unstd/string.cpp    \
		  srcs/server_functions.cpp \

SRCS_BONUS = srcs_bonus/main.cpp \
			 srcs_bonus/bot.cpp   \
			 srcs_bonus/logs.cpp   \
			 srcs_bonus/ansi.cpp    \

OBJ_DIR	= objs
OBJ_DIR_BONUS = objs_bonus

OBJS	= $(addprefix $(OBJ_DIR)/, $(SRCS:.cpp=.o))
OBJS_BONUS	= $(addprefix $(OBJ_DIR_BONUS)/, $(SRCS_BONUS:.cpp=.o))

CXX			= c++
FLAGS		?= true
CXXFLAGS	= -std=c++98 -I includes

ifeq ($(FLAGS), true)
	CXXFLAGS	+= -Wall -Wextra -Werror
endif

DEBUG		?= false
MODE		=  "release"

ifeq ($(DEBUG), true)
	CXXFLAGS += -g -D DEBUG
	MODE = "debug"
endif

RM		= rm -rf

$(OBJ_DIR)/%.o: %.cpp
	@printf "\e[1;32m[compiling "$(MODE)" {"$(CXX)"}...]\e[1;00m "$<"\n"
	@$(CXX) $(CXXFLAGS) $(COPTS) -c $< -o $@

$(OBJ_DIR_BONUS)/%.o: %.cpp
	@printf "\e[1;32m[compiling "$(MODE)" {"$(CXX)"}...]\e[1;00m "$<"\n"
	@$(CXX) $(CXXFLAGS) $(COPTS) -c $< -o $@

all: $(NAME)

bonus : $(NAME_BONUS)

$(NAME):	$(OBJ_DIR) $(OBJS)
	@printf "\e[1;32m[linking   "$(MODE)" {"$(CXX)"}...]\e[1;00m "$@"\n"
	@$(CXX) -o $(NAME) $(OBJS)
	@printf "\e[1;32m[build finished]\e[1;00m\n"

$(NAME_BONUS):		$(OBJ_DIR_BONUS) $(OBJS_BONUS)
	@printf "\e[1;32m[linking   "$(MODE)" {"$(CXX)"}...]\e[1;00m "$@"\n"
	@$(CXX) -o $(NAME_BONUS) $(OBJS_BONUS)
	@printf "\e[1;32m[build finished]\e[1;00m\n"

$(OBJ_DIR):
	@mkdir -p $(sort $(addprefix $(OBJ_DIR)/, $(dir $(SRCS))))

$(OBJ_DIR_BONUS):
	@mkdir -p $(sort $(addprefix $(OBJ_DIR_BONUS)/, $(dir $(SRCS_BONUS))))

clean:
	@$(RM) $(OBJ_DIR) $(OBJ_DIR_BONUS)

fclean:		clean
	@$(RM) $(NAME) $(NAME_BONUS)

re:			fclean all

.PHONY:		all bonus clean fclean re 
