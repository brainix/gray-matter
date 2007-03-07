#==============================================================================#
#	GNUmakefile							       #
#									       #
#	Copyright © 2005-2007, The Gray Matter Team, original authors.	       #
#==============================================================================#

# Your operating system.  This must be LINUX, OS_X, or WINDOWS.
# BSD is based on OS X.  If you insist on running BSD, set this to OS_X.  ;-)
PLAT = LINUX

CXX  = g++
LANG = -ansi
WARN = -Wall -Werror
OPTI = -O3 -fomit-frame-pointer
PREP = -D$(PLAT)
LINK = -lpthread
DIR  = -Iinc

OBJS = bin/board.o   \
       bin/history.o \
       bin/main.o    \
       bin/search.o  \
       bin/table.o   \
       bin/thread.o  \
       bin/xboard.o

all : bin/gray

clean :
	rm -f $(OBJS) bin/gray

bin/%.o : src/%.cpp
	$(CXX) -c -o $@ $< $(LANG) $(WARN) $(OPTI) $(PREP) $(DIR)

bin/gray : $(OBJS)
	$(CXX) -o $@ $(LANG) $(WARN) $(OPTI) $(PREP) $(LINK) $(DIR) $(OBJS)
