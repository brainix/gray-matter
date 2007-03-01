#==============================================================================#
#	GNUmakefile							       #
#									       #
#	Copyright © 2005-2007, The Gray Matter Team, original authors.	       #
#		All rights reserved.					       #
#==============================================================================#

CC   = g++
LANG = -ansi
WARN = -Wall -Werror
OPTI = -O3 -fomit-frame-pointer
LINK = -lpthread
DIR  = -Iinc

OBJS = bin/board.o   \
       bin/ffsll.o   \
       bin/history.o \
       bin/main.o    \
       bin/opening.o \
       bin/search.o  \
       bin/table.o   \
       bin/xboard.o

all : bin/gray

clean :
	rm -f $(OBJS) bin/gray

bin/%.o : src/%.cpp
	$(CC) -c -o $@ $< $(LANG) $(WARN) $(OPTI) $(DIR)

bin/gray : $(OBJS)
	$(CC) -o $@ $(LANG) $(WARN) $(OPTI) $(LINK) $(DIR) $(OBJS)
