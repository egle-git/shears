COMPAREHISTOS_O=CompareHistos.o 
COMPAREHISTOS=CompareHistos

CXXFLAGS += -I./ -fno-strict-aliasing -g
LDFLAGS += -g

CXXFLAGS += $(shell root-config --cflags)
LDFLAGS += $(shell root-config --ldflags)
LIBS += $(shell root-config --libs) 

all: $(COMPAREHISTOS)

clean:
	@rm -f *~ \#* *.o $(COMPAREHISTOS) > /dev/null

$(COMPAREHISTOS):	$(COMPAREHISTOS_O)
	$(CXX) $(CXXFLAGS) $(LIBS) $(LDFLAGS) $^ -o $@

