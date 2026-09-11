# **************************************************************************** #

NAME	:= tagger

# * DIRECTORIES ************************************************************** #

SDIR	:= src/
IDIR	:= inc/
ODIR	:= obj/

# * FILES ******************************************************************** #

SRCS	:=
vpath %.cpp $(SDIR)
SRCS	+= main.cpp
SRCS	+= Header.cpp
SRCS	+= ExtendedHeader.cpp
SRCS	+= Footer.cpp
SRCS	+= Frame.cpp

SRCS	+= UFID.cpp
SRCS	+= TIF.cpp
SRCS	+= WIF.cpp
SRCS	+= AENC.cpp
SRCS	+= APIC.cpp
SRCS	+= GEOB.cpp

SRCS	+= File.cpp

SRCS	+= utils.cpp

OBJS	:= $(SRCS:.cpp=.o)
OBJS	:= $(addprefix $(ODIR), $(OBJS))

DEPS	:= $(OBJS:.o=.d)

# * COMMANDS ***************************************************************** #

CXX		:= c++
RM		:= rm -rf
MKDIR	:= mkdir -p

# * FLAGS ******************************************************************** #

CXXFLAGS	:= -Wall -Werror -Wextra -std=c++98 -MMD -MP
CPPFLAGS	:= -I $(IDIR)

ifdef DEBUG
	CXXFLAGS	+= -g3
else
	CXXFLAGS	+= -O3
endif

# * RULES ******************************************************************** #

all: $(NAME)

$(NAME): $(ODIR) $(OBJS)
	$(CXX) $(LDFLAGS) -o $(NAME) $(OBJS)

$(ODIR):
	$(MKDIR) $(ODIR)

$(ODIR)%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $< -c -o $@

-include $(DEPS)

clean:
	$(RM) $(ODIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
