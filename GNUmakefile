#==============================================================================#
#	GNUmakefile							       #
#									       #
#	Copyright © 2005-2007, The Gray Matter Team, original authors.	       #
#==============================================================================#

# Your operating system.  This must be LINUX, OS_X, or WINDOWS.
# BSD is based on OS X.  If you insist on running BSD, set this to OS_X.  ;-)
PLAT = LINUX

# Your processor.  This must be i386, i486, i586, pentium, pentium-mmx, i686,
# pentiumpro, pentium2, pentium3, pentium3m, pentium-m, pentium4, pentium4m,
# prescott, nocona, k6, k6-2, k6-3, athlon, athlon-tbird, athlon-4, athlon-xp,
# athlon-mp, k8, opteron, athlon64, athlon-fx, winchip-c6, winchip2, c3, or
# c3-2.
ARCH = pentium3

CXX  = g++
LANG = -ansi
WARN = -Wall -Werror
OPTI = -O3 -fomit-frame-pointer
PREP = -D$(PLAT)
LINK = -lpthread
DIR  = -Iinc
MACH = -march=$(ARCH)

OBJS = bin/board_base.o      \
       bin/board_heuristic.o \
       bin/book.o            \
       bin/clock.o           \
       bin/library.o         \
       bin/main.o            \
       bin/search_base.o     \
       bin/search_bogo.o     \
       bin/search_mtdf.o     \
       bin/table.o           \
       bin/xboard.o

all : bin/gray

clean :
	rm -f $(OBJS) bin/gray

bin/%.o : src/%.cpp
	$(CXX) -c -o $@ $< $(LANG) $(WARN) $(OPTI) $(PREP) $(DIR) $(MACH)

bin/gray : $(OBJS)
	$(CXX) -o $@ $(LANG) $(WARN) $(OPTI) $(PREP) $(LINK) $(DIR) $(MACH) $(OBJS)
