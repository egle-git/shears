include Makefile.arch
#----------------------------------------------------------------------------------
#TESTS           := nTuple_analyzer_emu.C
#TESTS           := nTuple_analyzer_muon.C
TESTS           := multiple_xsec_analyzer.C
#TESTS           := zjanalyzr.C
#TESTS           := npileup.C
#TESTS           := parton_analyzer.C
TESTO           := $(TESTS:.C=.o)
#TEST            := nTuple_analyzer_emu
#TEST            := nTuple_analyzer_muon
TEST            := multiple_xsec_analyzer
#TEST            := zjanalyzr
#TEST            := npileup
#TEST            := parton_analyzer
#LD_LIBRARY_PATH  +=:/afs/cern.ch/user/b/bbilin/LHAPDF/lib/
OBJ             = $(TESTO)
PROGRAMS        = $(TEST)
CXXFLAGS      += 
CXXFLAGS      += -I/afs/cern.ch/user/b/bbilin/lhapdf-5.9.1/include/
#GLIBS         +=-I/afs/cern.ch/user/b/bbilin/lhapdf-5.9.1/include/
GLIBS         +=-L/afs/cern.ch/user/b/bbilin/LHAPDF/lib/ -lLHAPDF
#----------------------------------------------------------------------------------
.SUFFIXES: .$(SrcSuf) .$(ObjSuf) .$(DllSuf)

all:            $(PROGRAMS)

$(TEST):        $(TESTO)
		$(LD) $(LDFLAGS) $^ $(GLIBS) $(OutPutOpt)$@
		@echo "$@ done"

clean:
		@rm -f $(OBJ) $(TESTO) core *~ src/*~ include/*~

.$(SrcSuf).$(ObjSuf):
		$(CXX) $(CXXFLAGS) $(GLIBS) -c $< -o $@
