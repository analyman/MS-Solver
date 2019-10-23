LaTeX    = pdflatex
DOC      = solverdoc

CXX      = g++
CC       = gcc
CFLAGS   = -g -O0
CXXFLAGA = $(CFLAGS) -std=c++14

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

LATEXDOC:=./doc/LaTeX/solverdoc.tex

TOPDIR    := $(PWD)
libsrc    := $(shell find ./src \( -type f \) -regex ".*\.\(cpp\|c\|cxx\|cx\)")
lib       := libsmms.so
outputDIR := build_dir
objfile   := $(patsubst %.cpp,$(outputDIR)/%.o,$(shell echo $(libsrc) | grep -oe "[0-z]*\.cpp"))
headers   := $(shell find ./include \( -type f \) -regex ".*\/[0-z]*\.\(hpp\|h\)")

LIB: $(lib)

$(DOC): $(LATEXDOC)
	$(LaTeX) -jobname=solverdoc $(LATEXDOC)

$(lib): $(objfile)
	$(CXX) -shared -o $@ $^

$(objfile): $(libsrc) $(headers)
	@cd $(TOPDIR)
	@[ -d $(outputDIR) ] || mkdir $(outputDIR)
	cd $(outputDIR) && $(CXX) -fPIC -c $(CXXFLAGA) $(patsubst %,../%,$(libsrc)) && cd $(TOPDIR)

clean:
	@[ -d $(outputDIR) ] && $(rm) -r $(outputDIR)
	@[ -f $(lib) ] && $(rm) -r $(lib)
