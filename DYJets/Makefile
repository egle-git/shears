# ===============================
# Makefile for ZJets Analysis
# ===============================

# Determine if working at CERN on lxplus machine or at IIHE on m machines
HOST = $(shell hostname -f | grep cern)
ifneq ($(HOST),)
    HOST = "lxplus"
else
    HOST = "net3"
endif

# define LHAPDF library Path accroding to the host
ifeq ($(HOST), "lxplus")
  ifeq ($(LHAPDF_DATA_PATH),)
    LHAPDFPATH = /afs/cern.ch/cms/slc5_amd64_gcc434/external/lhapdf/5.8.5/
  else
    LHAPDFPATH = $(LHAPDF_DATA_PATH)/../../
  endif
else
  ifeq ($(HOST), "net3")
    LHAPDFPATH = /cvmfs/cms.cern.ch/slc6_amd64_gcc530/external/lhapdf/6.1.6-ikhhed2/
  endif
endif

# $(info $$LHAPDFPATH is [${LHAPDFPATH}])

# get Root compiler flags
ROOTGLIBS     = $(shell $(ROOTSYS)/bin/root-config --glibs)
ROOTCFLAGS    = $(shell $(ROOTSYS)/bin/root-config --cflags)

LIB = LHAPDF
LIBLIST = $(addprefix -l, $(LIB))

LIBDIR = RooUnfold $(LHAPDFPATH)lib
LIBDIRLIST = $(addprefix -L, $(LIBDIR))

RTL = -Wl,-rpath,
LIBDIRLISTRTL = $(addprefix $(RTL), $(LIBDIR))

SRCDIR = Sources/
INCDIR = -IIncludes/ -IRooUnfold/src -I$(LHAPDFPATH)include
MAINDIR = Main/

CXX           = g++ -std=c++0x
CXXFLAGS      = -fPIC
CXXFLAGS      += -g -O0
CXXFLAGS      += $(ROOTCFLAGS)
CXXFLAGS      += -MD -Wall
#CPPFLAGS      += $(INCDIR)

# list of all source files
SOURCES = \
ZJets_newformat.cc ArgParser.cc variablesOfInterestZJets.cc getFilesAndHistogramsZJets.cc \
ConfigVJets.cc HistoSetZJets.cc functions.cc UnfoldingZJets.cc \
RecoComparison.cc PlotSettings.cc muresolution_run2.cc rochcor2015.cc BLUEMeth.cc Combination.cc \
printTable.cc rootFunctions.cc fixYscale.cc SectionedConfig.cc Config.cc

PLOTRESPMATRIX_O=Sources/plotRespMatrix.o
PLOTRESPMATRIX=Main/plotRespMatrix

SRCLIST = $(addprefix $(SRCDIR), $(SOURCES))
OBJLIST = $(SRCLIST:.cc=.o)
MAINLIST = runCombination.d  runRecoComparison.d  runUnfoldingZJets.d  runZJets_newformat.d \
	Sources/testDecompVar.d Sources/testPrintTable.d runIntegralCheck.d plotCrossSections.d \
	testSectionConfig.d
MAINDEPS = $(addprefix $(MAINDIR), $(MAINLIST))
DEPS    = $(SRCLIST:.cc=.d)
DEPS    += $(MAINDEPS)

.PHONY: all clean

all: Main/runRecoComparison \
Main/runZJets_newformat \
Main/runUnfoldingZJets  \
Main/runCombination \
Main/testDecompVar \
Main/runIntegralCheck \
Main/plotCrossSections \
$(PLOTRESPMATRIX)

#in untested branch:
# runCombination \
# runVJets \
# runZJets \
# runMakeStatisticsTable \
# dispatcher

# ===========================================================================
%.o: %.cc
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(INCDIR)

#I'm not sure why this generic rule does not work...
#run%: run%.o $(OBJLIST) RooUnfold/libRooUnfold.so
#	$(CXX) -o $@ $(LIBDIRLIST) $(LIBDIRLISTRTL) $^ $(ROOTGLIBS) $(LIBLIST)

Main/runZJets_newformat: Main/runZJets_newformat.o $(OBJLIST) RooUnfold/libRooUnfold.so
	$(CXX) -o $@ $(LIBDIRLIST) $(LIBDIRLISTRTL) $^ $(ROOTGLIBS) $(LIBLIST)

Main/runRecoComparison: Main/runRecoComparison.o $(OBJLIST) RooUnfold/libRooUnfold.so
	$(CXX) -o $@ $(LIBDIRLIST) $(LIBDIRLISTRTL) $^ $(ROOTGLIBS) $(LIBLIST)

Main/runUnfoldingZJets: Main/runUnfoldingZJets.o $(OBJLIST) RooUnfold/libRooUnfold.so
	$(CXX) -o $@ $(LIBDIRLIST) $(LIBDIRLISTRTL) $^ $(ROOTGLIBS) $(LIBLIST)

Main/runCombination: Main/runCombination.o $(OBJLIST) RooUnfold/libRooUnfold.so 
	$(CXX) -o $@ $(LIBDIRLIST) $(LIBDIRLISTRTL) $^ $(ROOTGLIBS) $(LIBLIST)

Main/plotCrossSections: Main/plotCrossSections.o $(OBJLIST) RooUnfold/libRooUnfold.so 
	$(CXX) -o $@ $(LIBDIRLIST) $(LIBDIRLISTRTL) $^ $(ROOTGLIBS) $(LIBLIST)

Main/testDecompVar: Sources/testDecompVar.o Sources/variablesOfInterestZJets.o
	$(CXX) -o $@ $(LIBDIRLIST) $(LIBDIRLISTRTL) $^ $(ROOTGLIBS) $(LIBLIST)

testPrintTable: Sources/testPrintTable.o Sources/printTable.o
	$(CXX) -o $@ $(LIBDIRLIST) $(LIBDIRLISTRTL) $^ $(ROOTGLIBS) $(LIBLIST)

Main/runIntegralCheck: Main/runIntegralCheck.o $(OBJLIST) RooUnfold/libRooUnfold.so
	$(CXX) -o $@ $(LIBDIRLIST) $(LIBDIRLISTRTL) $^ $(ROOTGLIBS) $(LIBLIST)

Main/plotRespMatrix: $(PLOTRESPMATRIX_O) 
	$(CXX) $(ROOTGLIBS) $^ -o $@  

RooUnfold/libRooUnfold.so:
	$(MAKE) -C RooUnfold

testSectionConfig: testSectionConfig.cc
	$(CXX) -o $@ $(CXXFLAGS) $(INCDIR) $<

# ===========================================================================
clean:
	rm -f *.o Sources/*.o Main/*.o Main/*.d Main/plotRespMatrix Main/runIntegralCheck testPrintTable Main/testDecompVar Main/plotCrossSections Main/runCombination Main/runUnfoldingZJets Main/runRecoComparison Main/runZJets_newformat
	$(MAKE) -C RooUnfold clean

clean-dep:
	rm -f *.d Sources/*.d
	$(MAKE) -C RooUnfold clean

echo_%:
	@echo $* = $($*)

-include $(DEPS)
