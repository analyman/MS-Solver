YACC      = bison
LEX       = lex
CC        = gcc
CXX       = g++
CCFLAGS   = -g -O0 -Wall
CXXFLAGS  = $(CCFLAGS)
LINKFLAGS = -lstdc++

ifeq ($(OS), Windows_NT)
	cd=chdir
	cp=cp
	rm=del
	mv=move
else
	cd=cd
	cp=cp
	rm=rm -f
	mv=mv
endif

TOPDIR := $(PWD)

YACC_OUTPUT := y.tab.c
LEX_OUTPUT  := lex.yy.c

CXX_ROUTINE := mmm_hx.cxx
CXX_OBJ     := $(patsubst %.cxx,%.o,$(CXX_ROUTINE))
C_ROUTINE   := mmm_h.c
YACC_FILE   := mmm.y
LEX_FILE    := mmm.lex

TEST_MAIN := test_main.c
TEST_OUT  := parser

ALL: $(TEST_OUT)

$(TEST_OUT): $(TEST_MAIN) $(YACC_OUTPUT) $(LEX_OUTPUT) $(C_ROUTINE) $(CXX_OBJ)
	$(CC) $(CCFLAGS) -o $@ $^ $(LINKFLAGS)

$(CXX_OBJ): $(CXX_ROUTINE)
	$(CXX) $(CXXFLAGS) -c $^

$(YACC_OUTPUT): $(YACC_FILE)
	$(YACC) -d -o $@ $^

$(LEX_OUTPUT): $(LEX_FILE)
	$(LEX) $^

clean:
	$(rm) $(YACC_OUTPUT) $(LEX_OUTPUT) $(TEST_OUT) *.o $(patsubst %.c,%.h,$(YACC_OUTPUT))
