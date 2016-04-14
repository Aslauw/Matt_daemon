NAME		= Matt_daemon
CXXFLAGS	= -std=c++11 -Wall -Wextra -Werror -I include
LDFLAGS		=

FILES		=	main.cpp			\
				Daemon.cpp			\
				TintinReporter.cpp	\

SRC			= $(addprefix $(FILES), src/)
OBJ			= $(FILES:%.cpp=obj/%.o)

# Colors
NO_COLOR	= \x1b[0m
GREEN		= \x1b[32;01m
RED			= \x1b[31;01m
YELLOW		= \x1b[33;01m
GRAY		= \x1b[37;01m

.PHONY: all re clean fclean

all: $(NAME)

$(NAME): $(OBJ)
	@printf "$(GRAY)Creating $(RED)$(NAME)$(NO_COLOR)"
	@$(CXX) $(LDFLAGS) $(OBJ) -o $(NAME)
	@if [ -a $(NAME) ] ; \
	then \
		printf "\e[42G$(GREEN)✓$(NO_COLOR)\n" ; \
	fi;

obj/%.o: src/%.cpp include
	@mkdir -p obj
	@printf "$(YELLOW)$< $(NO_COLOR)"
	@$(CXX) $(CXXFLAGS) -c $< -o $@
	@if [ -a $@ ] ; \
	then \
		printf "\e[42G$(GREEN)✓$(NO_COLOR)\n" ; \
	fi;

clean:
	@printf "$(GRAY)Removing objects$(NO_COLOR)"
	@rm -rf obj
	@printf "\e[42G$(GREEN)✓$(NO_COLOR)\n"

fclean: clean
	@printf "$(GRAY)Removing $(RED)$(NAME)$(NO_COLOR)"
	@rm -f $(NAME)
	@printf "\e[42G$(GREEN)✓$(NO_COLOR)\n"

re: fclean all
