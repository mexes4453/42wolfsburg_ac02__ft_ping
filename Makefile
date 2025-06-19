
#=== COLORS ====
COL_Y = "\033[1;33m"
COL_P = "\033[1;35m"
COL_G = "\033[1;32m"
COL_D = "\033[0m"
#=============================
# VARIABLES - GENERIC
#=============================
VAR_TRASH = /dev/null
ARGS=
DIR_PROJECT = $(shell pwd)
DIR_SRC = $(DIR_PROJECT)/src
DIR_OBJ = $(DIR_PROJECT)/obj
DIR_DEP = $(DIR_PROJECT)/dep

SRCS_FILES_CPP:=

SRCS_FILES_C:=  main.c xnet.c xproto_ip.c xproto_icmp.c xnet_utils.c xapp.c \
                xtimer.c icmp_echo.c

OBJ_FILES:= $(SRCS_FILES_CPP:.cpp=.o) $(SRCS_FILES_C:.c=.o)
$(info $(OBJ_FILES))
OBJS := $(patsubst %, $(DIR_OBJ)/%, $(OBJ_FILES))
SRCS_C := $(patsubst %, $(DIR_SRC)/%, $(SRCS_FILES_C))
SRCS_CPP := $(patsubst %, $(DIR_SRC)/%, $(SRCS_FILES_CPP))
#$(info $(OBJ_FILES))
#$(info $(OBJS))


DEP_FILES:= $(OBJS:.o=.d)
DEPS := $(patsubst %, $(DIR_DEP)/%, $(OBJ_FILES))

MSG = 
REP = origin
NAME = ft__ping


CC = g++
GCC = gcc
STD = c++17
DEBUG = 
VALGRIND = valgrind -s --leak-check=full --show-leak-kinds=all --track-origins=yes
USR_LIB_PATH_XPNG = ./xlib/xpng
USR_LIB_PATH_XDIR = ./xlib/xdir
USR_LIB_XPNG=libxpng.a
USR_LIB_XDIR=libxdir.a

# FLAGS
CFLAGS = -Werror -Wall -Wextra \
		 -lrt \
		 #-DXAPP__DEBUG \
		 #-DICMP_ECHO__DEBUG \
		 #-DXNET__DEBUG \
		 #-DXNET_UTILS__DEBUG \
		 #-DXPROTO_IP__DEBUG \
         #-g -pthread \
		 # -lSDL2 -ldl -lGL -lz #-std=$(STD) 


LIBFLAGS_STATIC = #-L$(USR_LIB_PATH_XPNG) -lxpng \
                  #-L$(USR_LIB_PATH_XDIR) -lxdir \
                  #-L$(USR_LIB_PATH_XDIR) -lxstring \

DEPSFLAG =: -MM $($@:.o=.d)

# INCLUDE
INCLUDES = -I./ \
           -I./inc \
		   -I$(USR_LIB_PATH)/inc \
           -I./xlib/xdir/inc/ \
           -I./xlib/xutils/inc/ \
           #-I$(USR_LIB_PATH_PRINTF) 

#=== DEBUG ====
ifeq ($(DEBUG), 1)
	CFLAGS := $(CFLAGS) -g -D_DEBUG_=1
endif

	

# ==================
# FUNCTION MACROS
# ==================
define LIB_CLEAN
	@echo $(COL_B) == Removing library: $(2) $(COL_D)
	@rm -rf $(2)
	@cd $(1); make clean > $(VAR_TRASH); cd $(DIR_PROJECT)
endef
define LIB_MAKE
	@echo
	@echo $(COL_P)Compiling Library - $(2) $(COL_D)
	@cd $(1); make > $(VAR_TRASH); cp $(2) $(DIR_PROJECT);
endef
define MAKE_EXE
	$(1) ./$(NAME) $(ARGS)
endef




# =======
# RECIPES
# =======

NAME : all
all : $(NAME) 
$(NAME) : $(OBJS) # $(USR_LIB_XPNG) $(USR_LIB_XDIR) 
	@echo "\033[1;33mCompiling Executables: $(NAME) \033[0m"
	$(GCC) $^ $(LIBFLAGS_STATIC) $(CFLAGS) $(INCLUDES) -o $@ -Wl,-Map=$(NAME).map
	@echo; echo "\033[1;32mCompilation Successful. \033[0m"
	@echo; echo


-include $(DEPS)

# obj files output
$(DIR_OBJ)/%.o : $(DIR_SRC)/%.cpp
	@mkdir -p $(dir $@) 
	@mkdir -p $(DIR_DEP) 
	@echo
	@echo "\033[1;33mCompiling OBJ files \033[0m"
	$(CC) -c $^ $(CFLAGS) $(INCLUDES) -o $@
	$(CC) -MM $^ $(CFLAGS) -o $*.d
	@mv $*.d $(DIR_DEP)

# obj files output
$(DIR_OBJ)/%.o : $(DIR_SRC)/%.c
	@mkdir -p $(dir $@) 
	@mkdir -p $(DIR_DEP) 
	@echo
	@echo "\033[1;33mCompiling OBJ files \033[0m"
	$(GCC) -c $^ $(CFLAGS) $(INCLUDES) -o $@
	$(GCC) -MM $^ $(CFLAGS) -o $*.d
	@mv $*.d $(DIR_DEP)
	@echo


# ==================
# External libraries
# ==================
$(USR_LIB_XPNG) :
	$(call LIB_MAKE, $(USR_LIB_PATH_XPNG),$(USR_LIB_XPNG))

$(USR_LIB_XDIR) :
	$(call LIB_MAKE, $(USR_LIB_PATH_XDIR),$(USR_LIB_XDIR))


# remove all object files
fclean:
	rm -rf $(DIR_OBJ) $(DIR_DEP) *.map
#	$(call LIB_CLEAN, $(USR_LIB_PATH_XPNG),$(USR_LIB_XPNG))
#	$(call LIB_CLEAN, $(USR_LIB_PATH_XDIR),$(USR_LIB_XDIR))

# remove final target files
clean: fclean
	rm -rf $(NAME)

# recompile everything
re: clean all




# ====
# GIT 
# ====
#define comments
push:	clean
	$(info Pushing to Git Repository)
ifeq ($(REP), 42)
	@git push wolfsburg main
	@echo $(COL_G)Pushed to repo: $(REP)$(COL_D)
#	@git rm -f --cached *.pdf *.md			#	2> /dev/null
else 
	@git add ../*
	@git ls-files; git status
	@git commit -m "$(MSG)"
	@git push origin main
	@echo $(COL_G)Pushed to repo: origin$(COL_D)
	@echo $(COL_G)$(MSG)$(COL_D)
endif

#endef # comments



# ====
# TEST 
# ====
test: re
	@echo
	@echo $(COL_G)=== TEST === $(COL_D)
	$(call MAKE_EXE, $(VALGRIND))


# ===========
# RUN PROGRAM
# ===========
run: re
	@echo
	@echo $(COL_G)=== Executing Program === $(COL_D)
	$(call MAKE_EXE)


.PHONY : all fclean clean re push test run
