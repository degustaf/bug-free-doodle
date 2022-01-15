# Defines

ifeq ($(OSTYPE),cygwin)
	CLEANUP=rm -f
	CLEANDIR=rm -rf
	MKDIR=mkdir -p
	TARGET_EXTENSION=out
else ifeq ($(OS),Windows_NT)
	CLEANUP=del /F /Q
	CLEANDIR=rd /S /Q
	MKDIR=mkdir
	TARGET_EXTENSION=exe
else
	CLEANUP=rm -f
	CLEANDIR=rm -rf
	MKDIR=mkdir -p
	TARGET_EXTENSION=out
endif

ifndef CXX
	CXX = g++
endif

PATHS = 		src
SRCS =			$(wildcard $(PATHS)/*.cpp)
LINK = 			$(CXX)
CXX_STD =		c++11
# The standard can be set to c++03, c++11, c++14 (experimental), or c++17 (experimental)
CFLAGS =		-I$(PATHS) -Wall -Wextra -pedantic -std=$(CXX_STD) -D_POSIX_C_SOURCE=200809L

LDFLAGS =
LDLIBS =

COMPILE =		$(CXX) $(CFLAGS) -MT $@ -MP -MMD -MF $*.Td
OBJS =			$(patsubst %.cpp,%.o,$(SRCS))
POSTCOMPILE =	@mv -f $*.Td $*.d && touch $@

.PHONY: all
.PHONY: clean
.PRECIOUS: %.Td
.PRECIOUS: %.d
.PRECIOUS: %.o


# Rules

# all: CFLAGS += -DDEBUG -DTEST -g -fprofile-arcs
all: CFLAGS += -DDEBUG -DTEST -g
all: lox.$(TARGET_EXTENSION)


lox.$(TARGET_EXTENSION): $(OBJS)
	$(LINK) -o $@ $^ $(LDLIBS) $(LDFLAGS)

%.o: %.cpp
	$(COMPILE) -c $< -o $@
	$(POSTCOMPILE)

clean:
	$(CLEANUP) $(PATHS)/*.d
	$(CLEANUP) $(PATHS)/*.Td
	$(CLEANUP) $(PATHS)/*.o


include $(wildcard $(PATHS)/*.d)
