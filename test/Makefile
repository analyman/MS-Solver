CXX      = g++
CC       = gcc
CFLAGS   = -g -O0 -Wl
CXXFLAG  = $(CFLAGS) -std=c++14
LFLAGS   = -L../ -lsmms
DYN_LIBS = ../libsmms.so

ifeq ($(OS), Windows_NT)
	cd=chdir
	cp=cp
	rm=del
	rm_pdf=$(rm) *pdf 2> nul
	mv=move
else
	cd=cd
	cp=cp
	rm=rm -f
	rm_pdf=$(rm) *pdf
	mv=mv
endif

TOPDIR     := $(PWD)
srcfiles   := $(shell ls | grep -oe "[0-z]*\.\(cpp\|cxx\|c\\)")
__outfiles := $(shell echo $(srcfiles) | grep -oe "[0-z]*\.")
outfiles   := $(shell echo $(__outfiles) | grep -oe "[0-z]*")
objfiles   := $(patsubst %,%.o,$(outfiles))

all: $(outfiles)

$(outfiles): $(srcfiles) $(DYN_LIBS)
	$(CXX) $(LFLAGS) $(CXXFLAGS) -o $@ $(shell echo $^ | grep -oe "`echo $@ | cut -d. -f 1`\.\(cpp\|cxx\|c\)") $(DYN_LIBS)

$(DYN_LIBS): 
	@cd ../ && make LIB && cd $(TOPDIR)

clean:
	@$(rm) *.o 
	@$(rm) $(outfiles)
