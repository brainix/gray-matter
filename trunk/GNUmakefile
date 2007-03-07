#==============================================================================#
#	GNUmakefile							       #
#									       #
#	Copyright © 2005-2007, The Gray Matter Team, original authors.	       #
#==============================================================================#

# Platforms:
PLATFORM  = LINUX
#PLATFORM = OS_X
#PLATFORM = WINDOWS

CXX  = g++                      # C++ compiler
LANG = -ansi                    # Language options
WARN = -Wall -Werror            # Warning options
OPTI = -O3 -fomit-frame-pointer # Optimization options
PREP = -D$(PLATFORM)            # Preprocessor options
LINK = -lpthread                # Linker options
DIR  = -Iinc                    # Directory options

OBJS = bin/board.o   \
       bin/history.o \
       bin/main.o    \
       bin/search.o  \
       bin/table.o   \
       bin/xboard.o

all : bin/gray

clean :
	rm -f $(OBJS) bin/gray

bin/%.o : src/%.cpp
	$(CXX) -c -o $@ $< $(LANG) $(WARN) $(OPTI) $(PREP) $(DIR)

bin/gray : $(OBJS)
	$(CXX) -o $@ $(LANG) $(WARN) $(OPTI) $(PREP) $(LINK) $(DIR) $(OBJS)
