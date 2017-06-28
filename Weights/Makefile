GETPILEUP_O=GetPileup.o 
GETPILEUP=GetPileup

GETSF_O=GetSF.o
GETSF=GetSF

CXXFLAGS += -I./ -fno-strict-aliasing -g
LDFLAGS += -g

CXXFLAGS += $(shell root-config --cflags)
LDFLAGS += $(shell root-config --libs)

all: $(GETPILEUP) $(GETSF)

clean:
	@rm -f *~ \#* *.o $(GETPILEUP) $(GETSF) > /dev/null

$(GETPILEUP):	$(GETPILEUP_O)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

$(GETSF):	$(GETSF_O)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@
