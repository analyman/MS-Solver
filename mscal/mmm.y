/* calculator */
%{
#include "./mmm_h.h"

int yylex();
int yyerror(char* msg);

ASTNode* parser_astnode;

%}

%union {
    _Identify m_id;
    _Num      m_num;
    _Func     m_func;
    _Operator m_opr;
    _Common   m_com;
}

//     ^   */  +-  = 
%token <m_opr>  OPA PLUS OPC OPD MINUS
%token <m_id>   ID
%token <m_num>  NUMBER
%token <m_func> FUNC
// %token MATRIX
%token DELIM
%token LPARA
%token RPARA

%type <m_com> expr param param_list

%left  OPD
%left  OPC
%left  PLUS MINUS
%right OPA
%left  UMINUS

%start Start

%%

Start : expr {parser_astnode = $1.m_token_info.m_node;}
      | /* EMPTY */
      ;

expr : expr OPA expr 
     { 
        $$.m_token_info.m_node                = $2.m_token_info.m_node;
        $$.m_token_info.m_node->m_first_child = $1.m_token_info.m_node;
        $1.m_token_info.m_node->m_next        = $3.m_token_info.m_node;
        $1.m_token_info.m_node->m_parent      = $$.m_token_info.m_node;
        $3.m_token_info.m_node->m_parent      = $$.m_token_info.m_node;
     }
     | expr PLUS expr
     { 
        $$.m_token_info.m_node                = $2.m_token_info.m_node;
        $$.m_token_info.m_node->m_first_child = $1.m_token_info.m_node;
        $1.m_token_info.m_node->m_next        = $3.m_token_info.m_node;
        $1.m_token_info.m_node->m_parent      = $$.m_token_info.m_node;
        $3.m_token_info.m_node->m_parent      = $$.m_token_info.m_node;
     }
     | expr MINUS expr
     { 
        $$.m_token_info.m_node                = $2.m_token_info.m_node;
        $$.m_token_info.m_node->m_first_child = $1.m_token_info.m_node;
        $1.m_token_info.m_node->m_next        = $3.m_token_info.m_node;
        $1.m_token_info.m_node->m_parent      = $$.m_token_info.m_node;
        $3.m_token_info.m_node->m_parent      = $$.m_token_info.m_node;
     }
     | expr OPC expr
     { 
        $$.m_token_info.m_node                = $2.m_token_info.m_node;
        $$.m_token_info.m_node->m_first_child = $1.m_token_info.m_node;
        $1.m_token_info.m_node->m_next        = $3.m_token_info.m_node;
        $1.m_token_info.m_node->m_parent      = $$.m_token_info.m_node;
        $3.m_token_info.m_node->m_parent      = $$.m_token_info.m_node;
     }
     | ID   OPD expr
     { 
        $$.m_token_info.m_node                = $2.m_token_info.m_node;
        $$.m_token_info.m_node->m_first_child = $1.m_token_info.m_node;
        $1.m_token_info.m_node->m_next        = $3.m_token_info.m_node;
        $1.m_token_info.m_node->m_parent      = $$.m_token_info.m_node;
        $3.m_token_info.m_node->m_parent      = $$.m_token_info.m_node;
     }
     | MINUS expr
     { 
        $$.m_token_info.m_node                = $1.m_token_info.m_node;
        $$.m_token_info.m_node->m_first_child = $2.m_token_info.m_node;
        $2.m_token_info.m_node->m_parent      = $$.m_token_info.m_node;
     } %prec UMINUS
     | LPARA expr RPARA
     { 
        $$.m_token_info.m_node = $2.m_token_info.m_node;
     }
     | FUNC LPARA param_list RPARA
     { 
        $$.m_token_info.m_node                = $1.m_token_info.m_node;
        $1.m_token_info.m_node->m_first_child = $3.m_token_info.m_node;
        for(ASTNode* node = $1.m_token_info.m_node; node != NULL; node = node->m_next)
            node->m_parent = $$.m_token_info.m_node;
     }
     | FUNC LPARA RPARA
     {
        $$.m_token_info.m_node = $1.m_token_info.m_node;
     }
     | ID
     { 
        $$.m_token_info.m_node = $1.m_token_info.m_node;
     }
     | NUMBER
     { 
        $$.m_token_info.m_node = $1.m_token_info.m_node;
     }
     ;

param_list : param_list DELIM param
           { 
               $$.m_token_info.m_node->m_next = $3.m_token_info.m_node;
           }
           | param
           { 
               $$.m_token_info.m_node = $1.m_token_info.m_node;
           }
           ;

param : expr
      { 
          $$.m_token_info.m_node = $1.m_token_info.m_node;
      }
      ;

%%

int yyerror(char* msg)
{
    fprintf(stderr, "%s\n", msg);
    return -1;
}
