# **************************************************************************** #

NAME	:= tagger

# * DIRECTORIES ************************************************************** #

SDIR	:= src/
IDIR	:= inc/
ODIR	:= obj/

# * FILES ******************************************************************** #

SRCS	:=
vpath %.c $(SDIR)
SRCS	+= utils.c

SRCS	+= frame.c
SRCS	+= header.c
SRCS	+= ext_header.c
SRCS	+= footer.c
SRCS	+= file.c

SRCS	+= aenc.c
SRCS	+= apic.c
SRCS	+= geob.c
SRCS	+= ufid.c

SRCS	+= main.c

OBJS	:= $(SRCS:.c=.o)
OBJS	:= $(addprefix $(ODIR), $(OBJS))

DEPS	:= $(OBJS:.o=.d)

# * COMMANDS ***************************************************************** #

CC		:= gcc
LD		:= gcc
RM		:= rm -rf
MKDIR	:= mkdir -p

# * FLAGS ******************************************************************** #

CFLAGS		:= -Wall -Werror -Wextra -MMD -MP
CPPFLAGS	:= -I $(IDIR)

ifdef DEBUG
	CFLAGS	+= -g3
else
	# CFLAGS	+= -O3
endif

# * RULES ******************************************************************** #

all: $(NAME)

$(NAME): $(ODIR) $(OBJS)
	$(LD) $(LDFLAGS) -o $(NAME) $(OBJS)

$(ODIR):
	$(MKDIR) $(ODIR)

$(ODIR)%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $< -c -o $@

-include $(DEPS)

clean:
	$(RM) $(ODIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
