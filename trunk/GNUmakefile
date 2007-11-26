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

# Subversion macros
SVNDEF := -D'SVN_REV="$(shell svnversion -n .)"'

CXX  = g++
LANG = -ansi
WARN = -Wall #-Werror
OPTI = -g -O3 -fomit-frame-pointer $(SVNDEF)
PREP = -D$(PLAT)
LINK = -lpthread
DIR  = -Iinc
MACH = -march=$(ARCH)

SRCS = $(wildcard src/*.cpp)
OBJS = $(addprefix bin/,$(notdir $(subst .cpp,.o,$(SRCS))))
DEPS = $(addsuffix .d,$(basename $(OBJS)))

all : bin/gray

# If not cleaning, etc., use dependencies
ifeq ($(filter clean,$(MAKECMDGOALS)),)
-include $(DEPS)
endif

clean :
	rm -f $(DEPS) $(OBJS) bin/gray

bin/%.o : src/%.cpp
	$(CXX) -c -o $@ $< $(LANG) $(WARN) $(OPTI) $(PREP) $(DIR) $(MACH)

bin/gray : $(OBJS)
	@echo "OBJS = $(OBJS)"
	$(CXX) -o $@ $(LANG) $(WARN) $(OPTI) $(PREP) $(LINK) $(DIR) $(MACH) $(OBJS)

bin/%.d: src/%.cpp
	@set -e; $(CC) $(CPPFLAGS) -Iinc -DBUILDDEPS -MM $< \
		| sed 's/\($*\)\.o[ :]*/bin\/\1.o bin\/$*.d : /g' > $@; \
		[ -s $@ ] || rm -f $@


