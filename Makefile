CXX=clang++
RM=rm -f
CPPFLAGS=-g -std=c++14 -Iinclude
LDFLAGS=-lsdl2
BINARY=emu
SRCDIR=src
SRCEXT=cc
BUILDDIR=build
BINDIR=bin

SRCS=$(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
SRCOBJS=$(subst .$(SRCEXT),.o,$(SRCS))
OBJS=$(patsubst %.$(SRCEXT), %.o, $(SRCS))

all: build

$(OBJS): src/%.o : src/%.$(SRCEXT)
		@mkdir -p $(BUILDDIR)
		$(CXX) $(CPPFLAGS) -c $< -o $(subst src,build,$@)

build: $(SRCOBJS)
		@mkdir -p $(BINDIR)
		$(CXX) $(LDFLAGS) -o $(BINDIR)/$(BINARY) $(subst src,build,$(OBJS)) $(LDLIBS)

depend: .depend

.depend: $(SRCS)
		rm -f ./.depend
		$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
		$(RM) $(OBJS) *~ .depend

include .depend