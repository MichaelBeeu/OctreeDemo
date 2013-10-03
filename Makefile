SHELL=/bin/sh
CXX=g++
FLAGS = 
LIBS = gl sdl2 glew
CXXFLAGS = -g -Wall `sdl2-config --cflags`
LDFLAGS = 

BINDIR = bin
SRCDIR = src
INCDIR = include
TESTDIR = test

ifdef LIBS
	CXXFLAGS += `pkg-config --cflags $(LIBS)`
    LDFLAGS += `pkg-config --libs $(LIBS)`
endif

BINNAME=$(shell basename `pwd`)
TARGET=$(BINDIR)/$(BINNAME)
SOURCES=$(shell echo $(SRCDIR)/*.cpp)
CXXFLAGS+=-I$(INCDIR)/
OBJECTS=$(patsubst %.cpp,$(BINDIR)/%.o,$(SOURCES))
# OBJECTS=$(SOURCES:.cpp=.o)

all: $(TARGET)

-include $(OBJECTS:.o=.d)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(LDFLAGS) $(OBJECTS)

run: $(TARGET)
	$(TARGET)

dotest:
	@cd $(TESTDIR); make test

testclean:
	@cd $(TESTDIR); make clean

clean:
	-rm -rf $(OBJECTS)
	-rm -rf $(OBJECTS:.o=.d)
	-rm -f gmon.out

distlean: clean testclean
	-rm -f $(TARGET)

#%.o:%.cpp $$($$@_DEPS)
#	$(CC) -MD $(FLAGS) $(CFLAGS) $(HEADERS) -c -o $@ $<

$(BINDIR)/%.o : %.cpp
	-mkdir -p $(dir $@)
	$(CXX) -MMD $(CXXFLAGS) -c -o $@ $<

.PHONY: all
