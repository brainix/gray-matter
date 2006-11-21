#==============================================================================#
#	GNUmakefile							       #
#									       #
#	Copyright © 2005-2006, The Gray Matter Team, original authors.	       #
#		All rights reserved.					       #
#==============================================================================#

CC   = g++
LANG = -ansi
WARN = -Wall -Werror
OPTI = -O3 -fomit-frame-pointer
LINK = -lpthread
DIR  = -Iinc
I386 = -march=pentium-mmx

OBJS = bin/board.o   \
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
	$(CC) -c -o $@ $< $(LANG) $(WARN) $(OPTI) $(DIR) $(I386)

bin/gray : $(OBJS)
	$(CC) -o $@ $(LANG) $(WARN) $(OPTI) $(LINK) $(DIR) $(I386) $(OBJS)
