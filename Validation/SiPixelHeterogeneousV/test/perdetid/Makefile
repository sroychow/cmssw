UNAME    = $(shell uname)
EXE      = digiComp.exe clusterComp.exe rechitComp.exe
 
#vpath %.h ./interface

CSUF   = cc
#HSUF   = h
DICTC  = Dict.cc
#DICTH  = $(patsubst %.$(CSUF),%.h,$(DICTC))

SRCS   = digiComp.C rechitComp.C clusterComp.C   
#OBJS   = $(patsubst %.$(CSUF), %.o, $(SRCS))

CXX       = g++
CXXFLAGS += -g `root-config --libs --cflags`

HDRS_DICT = DetInfo.h LinkDef.h

bin: digiComp.exe clusterComp.exe rechitComp.exe
all: 
	gmake cling 
	gmake bin 
cint: $(DICTC) 

$(DICTC): $(HDRS_DICT)
	@echo "Generating dictionary $(DICTC) and $(DICTH) ..."
	rootcling -f $@ $^
digiComp.exe: digiComp.C Dict.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS) `root-config --libs`

clusterComp.exe: clusterComp.C Dict.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS) `root-config --libs`

rechitComp.exe: rechitComp.C Dict.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS) `root-config --libs`

# Clean 
.PHONY   : clean 
clean : 
	@-rm $(EXE) $(DICTC) *.o  

