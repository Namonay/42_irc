#******************************************************************************#
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaas <vvaas@student.42angouleme.fr>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/08/09 15:08:49 by vavaas            #+#    #+#              #
#    Updated: 2024/01/22 17:32:49 by maldavid         ###   ########.fr        #
#                                                                              #
#******************************************************************************#

NAME	= ircserv

SRCS	= srcs/main.cpp \
		  srcs/logs.cpp  \
		  srcs/server.cpp  \
		  srcs/server_functions.cpp  \
		  srcs/client.cpp  \
		  srcs/channel.cpp  \
		  srcs/message.cpp  \
		  srcs/ansi.cpp  \

OBJ_DIR	= objs
OBJS	= $(addprefix $(OBJ_DIR)/, $(SRCS:.cpp=.o))

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

all: $(NAME)

$(NAME):	$(OBJ_DIR) $(OBJS)
	@printf "\e[1;32m[linking   "$(MODE)" {"$(CXX)"}...]\e[1;00m "$@"\n"
	@$(CXX) -o $(NAME) $(OBJS)
	@printf "\e[1;32m[build finished]\e[1;00m\n"

$(OBJ_DIR):
	@mkdir -p $(sort $(addprefix $(OBJ_DIR)/, $(dir $(SRCS))))

clean:
	@$(RM) $(OBJ_DIR)

fclean:		clean
	@$(RM) $(NAME)

re:			fclean all

.PHONY:		all clean fclean re 
