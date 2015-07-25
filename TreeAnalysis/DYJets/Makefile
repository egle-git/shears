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

# define LHAPDF library Path accroding to the host 
ifeq ($(HOST), "lxplus")
  LHAPDFPATH = /afs/cern.ch/cms/slc5_amd64_gcc434/external/lhapdf/5.8.5/
else
  ifeq ($(HOST), "iihe")
    LHAPDFPATH = /user/aleonard/LHAPDF/
  endif
endif

# get Root compiler flags
ROOTGLIBS     = $(shell $(ROOTSYS)/bin/root-config --glibs)
ROOTCFLAGS    = $(shell $(ROOTSYS)/bin/root-config --cflags) 

LIB = RooUnfold LHAPDF
LIBLIST = $(addprefix -l, $(LIB))

LIBDIR = RooUnfold $(LHAPDFPATH)lib
LIBDIRLIST = $(addprefix -L, $(LIBDIR))

RTL = -Wl,-rpath,
LIBDIRLISTRTL = $(addprefix $(RTL), $(LIBDIR))

SRCDIR = Sources/
INCDIR = -IIncludes/ -IRooUnfold/src -I$(LHAPDFPATH)include

CXX           = g++ -std=c++0x
CXXFLAGS      = -g -Wall -fPIC   	
CXXFLAGS      += $(ROOTCFLAGS) 

# list of all source files
SOURCES = ArgParser.cc variablesOfInterestZJets.cc getFilesAndHistogramsZJets.cc ConfigVJets.cc HistoSetZJets.cc ZJets.cc functions.cc
SOURCES += MergeTop.cc standalone_LumiReWeighting.cc UnfoldingZJets.cc RecoComparison.cc
SOURCES += PlotSettings.cc BLUEMeth.cc Combination.cc makeStatisticsTable.cc rochcor.cc

SRCLIST = $(addprefix $(SRCDIR), $(SOURCES))
OBJLIST = $(SRCLIST:.cc=.o)

.PHONY: all clean

all: runCombination \
runRecoComparison \
runVJets \
runZJets \
runUnfoldingZJets \
runMakeStatisticsTable \
dispatcher

# ===========================================================================
.cc.o:
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(INCDIR)

runVJets: $(OBJLIST) runVJets.o RooUnfold/libRooUnfold.so
	$(CXX) -o runVJets $(LIBDIRLIST) $(LIBDIRLISTRTL) $^ $(ROOTGLIBS) $(LIBLIST)

runVJets.o: runVJets.cc
	$(CXX) -o $@ -c -IIncludes $< $(CXXFLAGS) $(INCDIR) 

runZJets: $(OBJLIST) runZJets.o RooUnfold/libRooUnfold.so
	$(CXX) -o runZJets $(LIBDIRLIST) $(LIBDIRLISTRTL) $^ $(ROOTGLIBS) $(LIBLIST)

runZJets.o: runZJets.cc
	$(CXX) -o $@ -c -IIncludes $< $(CXXFLAGS) $(INCDIR) 

runRecoComparison: $(OBJLIST) runRecoComparison.o  RooUnfold/libRooUnfold.so
	$(CXX) -o runRecoComparison $(LIBDIRLIST) $(LIBDIRLISTRTL) $^ $(ROOTGLIBS) $(LIBLIST)

runRecoComparison.o: runRecoComparison.cc
	$(CXX) -o $@ -c -IIncludes $< $(CXXFLAGS) $(INCDIR)

runUnfoldingZJets: $(OBJLIST) runUnfoldingZJets.o  RooUnfold/libRooUnfold.so
	$(CXX) -o runUnfoldingZJets $(LIBDIRLIST) $(LIBDIRLISTRTL) $^ $(ROOTGLIBS) $(LIBLIST)

runUnfoldingZJets.o: runUnfoldingZJets.cc RooUnfold/libRooUnfold.so
	$(CXX) -o $@ -c -IIncludes $< $(CXXFLAGS) $(INCDIR)

runCombination: $(OBJLIST) runCombination.o
	$(CXX) -o runCombination $(LIBDIRLIST) $(LIBDIRLISTRTL) $^ $(ROOTGLIBS) $(LIBLIST)

runCombination.o: runCombination.cc
	$(CXX) -o $@ -c -IIncludes $< $(CXXFLAGS) $(INCDIR)

runMakeStatisticsTable: $(OBJLIST) runMakeStatisticsTable.o
	$(CXX) -o runMakeStatisticsTable $(LIBDIRLIST) $(LIBDIRLISTRTL) $^ $(ROOTGLIBS) $(LIBLIST)

runMakeStatisticsTable.o: runMakeStatisticsTable.cc RooUnfold/libRooUnfold.so
	$(CXX) -o $@ -c -IIncludes $< $(CXXFLAGS) $(INCDIR)

dispatcher: $(OBJLIST) dispatcher.o  RooUnfold/libRooUnfold.so
	$(CXX) -o dispatcher $(LIBDIRLIST) $(LIBDIRLISTRTL) $^ $(ROOTGLIBS) $(LIBLIST)

dispatcher.o: dispatcher.cc
	$(CXX) -o $@ -c -IIncludes $< $(CXXFLAGS) $(INCDIR)

RooUnfold/libRooUnfold.so:
	$(MAKE) -C RooUnfold

# =========================================================================== 
clean: 
	rm -f *.o Sources/*.o 



