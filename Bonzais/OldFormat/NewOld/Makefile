# =============================== 
# Makefile for ZJets Analysis 
# =============================== 

# Determine if working at CERN on lxplus machine or at IIHE on m machines
HOST = $(shell hostname | grep lxplus)

ifeq ($(HOST),)
  HOST = $(shell hostname | grep iihe)
  ifeq ($(HOST),)
  else
    HOST = "iihe"
  endif
else
  HOST = "lxplus"
endif

# get Root compiler flags
ROOTGLIBS     = $(shell $(ROOTSYS)/bin/root-config --glibs)
ROOTCFLAGS    = $(shell $(ROOTSYS)/bin/root-config --cflags) 

RTL = -Wl,-rpath,
LIBDIRLISTRTL = $(addprefix $(RTL), $(LIBDIR))

SRCDIR = src/
INCDIR = -Iinclude/ 

CXX           = g++ -std=c++0x
CXXFLAGS      = -g -Wall -fPIC   	
CXXFLAGS      += $(ROOTCFLAGS) 

# list of all source files
SOURCES = BonzaiMaker.cc ArgParser.cc ConfigVJets.cc

SRCLIST = $(addprefix $(SRCDIR), $(SOURCES))
OBJLIST = $(SRCLIST:.cc=.o)

.PHONY: all clean

all: runMyTree

# ===========================================================================
.cc.o:
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(INCDIR)

runMyTree: $(OBJLIST) runMyTree.o
	$(CXX) -o runMyTree $(LIBDIRLIST) $(LIBDIRLISTRTL) $^ $(ROOTGLIBS) $(LIBLIST)

runMyTree.o: runMyTree.cc
	$(CXX) -o $@ -c -IIncludes $< $(CXXFLAGS) $(INCDIR) 

