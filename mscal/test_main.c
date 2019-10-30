#include <stdio.h>
#include "mmm_h.h"

extern FILE* yyin;
int yyparse();

int main()
{
    yyin = stdin;
    while(1){
        printf("yyparse() is called.\n"); 
        yyparse(); 
        show_context(); 
        printf("number of node be deleted %d\n", delete_astnode(parser_astnode));
    }
    return 0;
}
