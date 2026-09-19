ET_DIR		:= et

TEST_DIR	:= .

PROJ_DIR	:= ..
PROJ_IDIR	:= $(PROJ_DIR)/inc

LIB_DIRS	:= $(PROJ_DIR)/build
vpath %.a $(LIB_DIRS)
LIB_DIRS	:= $(addprefix -L,$(LIB_DIRS))

LIBS		:= id3v240.a
LIBS		:= $(addprefix -l:,$(LIBS))

VPATH		:= $(TEST_DIR) $(ET_DIR)

INCLUDES	:= $(TEST_DIR) $(PROJ_IDIR) $(ET_DIR)
INCLUDES	:= $(addprefix -I,$(INCLUDES))

# * PROJECT FILES ************************************************************ #

SRCS			:= et.c et_host.c

# * TOOLS ******************************************************************** #

CC			:= cc
LINK			:= cc

ifeq ($(OS),Windows_NT)
	MKDIR      := mkdir
	RM         := rm
	TARGET_EXT := .exe
else ifeq ($(OSTYPE),cygwin)
	MKDIR      := mkdir -p
	RM         := rm -f
	TARGET_EXT := .exe
else
	MKDIR      := mkdir -p
	RM         := rm -f
	TARGET_EXT :=
endif

# * BUILD OPTIONS ************************************************************ #

BIN_DIR ?= build

CFLAGS  := -c -g -O -fno-pie -std=c11 -pedantic -Wall -Wextra -W \
	$(INCLUDES) $(DEFINES) -DQ_HOST

ifdef GCOV
	CFLAGS    += -fprofile-arcs -ftest-coverage
	CPPFLAGS  += -fprofile-arcs -ftest-coverage
	LINKFLAGS += -lgcov --coverage
endif

OBJS       := $(patsubst %.c,%.o,   $(SRCS))

OBJS_EXT   := $(addprefix $(BIN_DIR)/, $(OBJS))
DEPS_EXT   := $(patsubst %.o,%.d, $(OBJS_EXT))

FRAME_EXE   := $(BIN_DIR)/frame$(TARGET_EXT)
FRAME_SRCS		:= frame_test.c
FRAME_OBJS 		:= $(patsubst %.c,%.o,   $(FRAME_SRCS))
FRAME_OBJS_EXT   := $(addprefix $(BIN_DIR)/, $(FRAME_OBJS))
FRAME_DEPS_EXT   := $(patsubst %.o,%.d, $(FRAME_OBJS_EXT))

UTILS_EXE   := $(BIN_DIR)/utils$(TARGET_EXT)
UTILS_SRCS		:= utils_test.c
UTILS_OBJS := $(patsubst %.c,%.o,   $(UTILS_SRCS))
UTILS_OBJS_EXT   := $(addprefix $(BIN_DIR)/, $(UTILS_OBJS))
UTILS_DEPS_EXT   := $(patsubst %.o,%.d, $(UTILS_OBJS_EXT))

# * RULES ******************************************************************** #

.PHONY : norun clean show

ifeq ($(MAKECMDGOALS),norun)
all : frame utils
norun : all
else
all : frame utils run
endif

frame : id3v240.a $(FRAME_EXE)

$(FRAME_EXE) : $(OBJS_EXT) $(FRAME_OBJS_EXT)
	$(LINK) $(LINKFLAGS) $(LIB_DIRS) -o $@ $^ $(LIBS)

utils : id3v240.a $(UTILS_EXE)

$(UTILS_EXE) : $(OBJS_EXT) $(UTILS_OBJS_EXT)
	$(LINK) $(LINKFLAGS) $(LIB_DIRS) -o $@ $^ $(LIBS)

run : $(FRAME_EXE) $(UTILS_EXE)
	$(foreach exe,$^,$(exe);)

id3v240.a :
	$(MAKE) -C $(PROJ_DIR)

$(BIN_DIR)/%.d : %.c
	$(CC) -MM -MT $(@:.d=.o) $(CFLAGS) $< > $@

$(BIN_DIR)/%.o : %.c
	$(CC) $(CFLAGS) $< -o $@

# create BIN_DIR and include dependencies only if needed
ifneq ($(MAKECMDGOALS),clean)
  ifneq ($(MAKECMDGOALS),show)
     ifneq ($(MAKECMDGOALS),debug)
ifeq ("$(wildcard $(BIN_DIR))","")
$(shell $(MKDIR) $(BIN_DIR))
endif
-include $(DEPS_EXT)
-include $(FRAME_DEPS_EXT)
-include $(UTILS_DEPS_EXT)
     endif
  endif
endif

clean :
	-$(RM) $(BIN_DIR)/*.*

show :
	@echo PROJECT		= $(PROJECT)
	@echo TARGET_EXE	= $(TARGET_EXE)
	@echo VPATH		= $(VPATH)
	@echo SRCS		= $(SRCS)
	@echo DEPS_EXT		= $(DEPS_EXT)
	@echo OBJS_EXT  	= $(OBJS_EXT)
	@echo DEPS_EXT   	= $(C_DEPS_EXT)
	@echo LDIR  	  	= $(LIB_DIRS)
	@echo LIBS		= $(LIBS)
	@echo DEFINES     	= $(DEFINES)
	@echo INCLUDES		= $(INCLUDES)
