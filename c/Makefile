# **************************************************************************** #

LIBNAME	:= id3v240.a
NAME	:= tagger

# * DIRECTORIES ************************************************************** #

SDIR	:= src/
IDIR	:= inc/
BDIR	:= build/

LIBNAME	:= $(addprefix $(BDIR), $(LIBNAME))
NAME	:= $(addprefix $(BDIR), $(NAME))

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
SRCS	+= tif.c
SRCS	+= txxx.c

PROG_SRCS	:= $(SRCS)
PROG_SRCS	+= main.c

OBJS	:= $(SRCS:.c=.o)
OBJS	:= $(addprefix $(BDIR), $(OBJS))

PROG_OBJS	:= $(PROG_SRCS:.c=.o)
PROG_OBJS	:= $(addprefix $(BDIR), $(PROG_OBJS))


DEPS	:= $(PROG_OBJS:.o=.d)

# * COMMANDS ***************************************************************** #

CC		:= gcc
LD		:= gcc
AR		:= ar
RM		:= rm -rf
MKDIR		:= mkdir -p

# * FLAGS ******************************************************************** #

CFLAGS		?=
CFLAGS		+= -Wall -Werror -Wextra -MMD -MP
CPPFLAGS	?=
CPPFLAGS	+= -I $(IDIR)
ARFLAGS		:= rcs

ifdef DEBUG
	CFLAGS	+= -g3
else
	CFLAGS	+= -O3
endif

# * RULES ******************************************************************** #

all: $(LIBNAME)

$(LIBNAME): $(BDIR) $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

$(NAME): $(BDIR) $(PROG_OBJS)
	$(LD) $(LDFLAGS) -o $(NAME) $(OBJS)

$(BDIR):
	$(MKDIR) $(BDIR)

$(BDIR)%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $< -c -o $@

-include $(DEPS)

clean:
	$(RM) $(BDIR)

re: fclean all

.PHONY: all clean fclean re
