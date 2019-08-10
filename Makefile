LaTeX=pdflatex
DOC=solverdoc

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

LATEXDOC=./doc/LaTeX/solverdoc.tex

$(DOC): $(LATEXDOC)
	$(LaTeX) -jobname=solverdoc $(LATEXDOC)