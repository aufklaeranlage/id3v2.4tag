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

HEADER_EXE   := $(BIN_DIR)/header$(TARGET_EXT)
HEADER_SRCS		:= header_test.c
HEADER_OBJS := $(patsubst %.c,%.o,   $(HEADER_SRCS))
HEADER_OBJS_EXT   := $(addprefix $(BIN_DIR)/, $(HEADER_OBJS))
HEADER_DEPS_EXT   := $(patsubst %.o,%.d, $(HEADER_OBJS_EXT))

EXT_HEADER_EXE   := $(BIN_DIR)/ext_header$(TARGET_EXT)
EXT_HEADER_SRCS		:= ext_header_test.c
EXT_HEADER_OBJS := $(patsubst %.c,%.o,   $(EXT_HEADER_SRCS))
EXT_HEADER_OBJS_EXT   := $(addprefix $(BIN_DIR)/, $(EXT_HEADER_OBJS))
EXT_HEADER_DEPS_EXT   := $(patsubst %.o,%.d, $(EXT_HEADER_OBJS_EXT))

FOOTER_EXE   := $(BIN_DIR)/footer$(TARGET_EXT)
FOOTER_SRCS		:= footer_test.c
FOOTER_OBJS := $(patsubst %.c,%.o,   $(FOOTER_SRCS))
FOOTER_OBJS_EXT   := $(addprefix $(BIN_DIR)/, $(FOOTER_OBJS))
FOOTER_DEPS_EXT   := $(patsubst %.o,%.d, $(FOOTER_OBJS_EXT))

# * RULES ******************************************************************** #

.PHONY : norun clean show

ifeq ($(MAKECMDGOALS),norun)
all : frame header ext_header footer utils
norun : all
else
all : frame header ext_header footer utils run
endif

frame : id3v240.a $(FRAME_EXE)

$(FRAME_EXE) : $(OBJS_EXT) $(FRAME_OBJS_EXT)
	$(LINK) $(LINKFLAGS) $(LIB_DIRS) -o $@ $^ $(LIBS)

header : id3v240.a $(HEADER_EXE)

$(HEADER_EXE) : $(OBJS_EXT) $(HEADER_OBJS_EXT)
	$(LINK) $(LINKFLAGS) $(LIB_DIRS) -o $@ $^ $(LIBS)

ext_header : id3v240.a $(EXT_HEADER_EXE)

$(EXT_HEADER_EXE) : $(OBJS_EXT) $(EXT_HEADER_OBJS_EXT)
	$(LINK) $(LINKFLAGS) $(LIB_DIRS) -o $@ $^ $(LIBS)

footer : id3v240.a $(FOOTER_EXE)

$(FOOTER_EXE) : $(OBJS_EXT) $(FOOTER_OBJS_EXT)
	$(LINK) $(LINKFLAGS) $(LIB_DIRS) -o $@ $^ $(LIBS)

utils : id3v240.a $(UTILS_EXE)

$(UTILS_EXE) : $(OBJS_EXT) $(UTILS_OBJS_EXT)
	$(LINK) $(LINKFLAGS) $(LIB_DIRS) -o $@ $^ $(LIBS)

run : $(FRAME_EXE) $(HEADER_EXE) $(EXT_HEADER_EXE) $(FOOTER_EXE) $(UTILS_EXE)
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
